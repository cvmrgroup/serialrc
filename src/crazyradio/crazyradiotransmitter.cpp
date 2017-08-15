//
// Created by lasse on 29.09.15.
//

#include "crazyradiotransmitter.h"

CrazyRadioTransmitter::CrazyRadioTransmitter(const std::string name, boost::shared_ptr<boost::asio::io_service> ioService)
        :
        name(name),
        ioService(ioService),
        running(false),
        currentRadio("-1")
{
}

void CrazyRadioTransmitter::open()
{
    // check if the thread already created
    if (this->thread)
    {
        return;
    }

    this->running = true;
    this->thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&CrazyRadioTransmitter::run, this)));
}

void CrazyRadioTransmitter::close()
{
    this->running = false;

    if (this->thread && this->thread->joinable())
    {
        this->thread->interrupt();
        this->thread->join();
    }
}

// /////////////////////////////////////////////////////////////////////////////

void CrazyRadioTransmitter::run()
{
    BOOST_LOG_TRIVIAL(info) << "Starting crazyradio thread...";

    this->initialize();

    if (!this->running)
    {
        return;
    }

    this->copter->logReset();

    std::function<void(float)> cb_lq = std::bind(&CrazyRadioTransmitter::onLinkQuality, this, std::placeholders::_1);
    this->copter->setLinkQualityCallback(cb_lq);

    std::unique_ptr<LogBlock<logImu> > logBlockImu;
    std::unique_ptr<LogBlock<log2> > logBlock2;
    //std::vector<std::unique_ptr<LogBlockGeneric> > logBlocksGeneric(m_logBlocks.size());

    if (this->enableLogging)
    {
        std::function<void(const crtpPlatformRSSIAck *)> cb_ack = std::bind(&CrazyRadioTransmitter::onCfEmptyAck, this, std::placeholders::_1);
        this->copter->setEmptyAckCallback(cb_ack);

        std::cout << "Requesting Logging variables..." << std::endl;
        this->copter->requestLogToc();

        if (this->enableLoggingImu)
        {
            std::function<void(uint32_t, logImu *)> cb = std::bind(&CrazyRadioTransmitter::onCfImuData, this, std::placeholders::_1, std::placeholders::_2);

            logBlockImu.reset(new LogBlock<logImu>(
                    this->copter, {
                            {"acc",  "x"},
                            {"acc",  "y"},
                            {"acc",  "z"},
                            {"gyro", "x"},
                            {"gyro", "y"},
                            {"gyro", "z"},
                    }, cb));
            logBlockImu->start(1); // 10ms
        }

        if (this->enableLoggingTemperature
            || this->enableLoggingMagneticField
            || this->enableLoggingPressure
            || this->enableLoggingBattery)
        {
            std::function<void(uint32_t, log2 *)> cb2 = std::bind(&CrazyRadioTransmitter::onCfLog2Data, this, std::placeholders::_1, std::placeholders::_2);

            logBlock2.reset(new LogBlock<log2>(
                    this->copter, {
                            {"mag",  "x"},
                            {"mag",  "y"},
                            {"mag",  "z"},
                            {"baro", "temp"},
                            {"baro", "pressure"},
                            {"pm",   "vbat"},
                    }, cb2));
            logBlock2->start(100); // 1000ms
        }

        // custom log blocks
        /*size_t i = 0;
        for (auto &logBlock : m_logBlocks)
        {
            std::function<void(uint32_t, std::vector<double> *, void *userData)> cb =
                    std::bind(
                            &CrazyflieROS::onLogCustom,
                            this,
                            std::placeholders::_1,
                            std::placeholders::_2,
                            std::placeholders::_3);

            logBlocksGeneric[i].reset(new LogBlockGeneric(
                    &m_cf,
                    logBlock.variables,
                    (void *) &m_pubLogDataGeneric[i],
                    cb));
            logBlocksGeneric[i]->start(logBlock.frequency / 10);
            ++i;
        }*/
    }

    std::cout << "Ready..." << std::endl;

    // Send 0 thrust initially for thrust-lock
    for (int i = 0; i < 100; ++i)
    {
        this->copter->sendSetpoint(0, 0, 0, 0);
    }

    try
    {
        while (this->running)
        {
            this->update();
            boost::this_thread::sleep(boost::posix_time::milliseconds(1));
        }
    }
    catch (boost::thread_interrupted &)
    {
        BOOST_LOG_TRIVIAL(error) << "Crazyradio thread interrupted.";
    }

    // Make sure we turn the engines off
    for (int i = 0; i < 100; ++i)
    {
        this->copter->sendSetpoint(0, 0, 0, 0);
    }

    // clean up the crazy radio
    this->cleanup();

    // display info message
    BOOST_LOG_TRIVIAL(info) << "Crazyradio thread stopped.";
}

void CrazyRadioTransmitter::initialize()
{
    // Fix Qt Issue with Localized String and sscanf
    // see: http://www.solved.online/311063/why-does-qt-change-behaviour-of-sscanf
    setlocale(LC_NUMERIC, "C");

    std::string uri = this->currentRadio;

    BOOST_LOG_TRIVIAL(info) << "Connecting crazyflie with crazyradio url [ " << uri << " ].";

    this->enableLogging = true;
    this->enableLoggingImu = true;
    this->enableLoggingTemperature = false;
    this->enableLoggingMagneticField = false;
    this->enableLoggingPressure = true;
    this->enableLoggingBattery = true;

    try
    {
        this->copter = new Crazyflie(uri);
    }
    catch (std::exception &e)
    {
        this->running = false;

        // create and throw radio exception
        std::string ex = boost::str(boost::format("Failed to open crazy radio with url [ %1% ].") % uri);
        BOOST_LOG_TRIVIAL(error) << ex;
        throw RadioException(ex);
    }
}

void CrazyRadioTransmitter::update()
{
    bool success = false;

    // the control signals
    cv::Vec4d u(0., 0., 0., 0.);

    // the latch to wait for the request
    boost::latch latch(2);

    // invoke the radio control request inside the io_service to be thread save
    this->ioService->post([this, &latch, &success, &u]()
                          {
                              // check if current radio exists
                              if (this->radios.find(this->currentRadio) != this->radios.end())
                              {
                                  AbstractTxModule *radio = this->radios[this->currentRadio];

                                  u = cv::Vec4d(radio->getThrottle(),
                                                radio->getRoll(),
                                                radio->getPitch(),
                                                radio->getYaw());

                                  success = true;
                              }

                              latch.count_down();
                          });

    latch.count_down_and_wait();

    if (success)
    {
        this->copter->sendSetpoint((float) u[1], (float) u[2], (float) u[3], (uint16_t) u[0]);
    }
    else
    {
        this->copter->sendPing();
    }
}

void CrazyRadioTransmitter::cleanup()
{
    this->copter = nullptr;
}

// /////////////////////////////////////////////////////////////////////////////

void CrazyRadioTransmitter::onCfEmptyAck(const crtpPlatformRSSIAck *data)
{
    if (this->enableLogging)
    {
        // dB
        //std::cout << "empty ack " << data->rssi << std::endl;
    }
}

void CrazyRadioTransmitter::onCfImuData(uint32_t time_in_ms, logImu *data)
{
    ImuData imuData;

    imuData.millis = Clock::nowMillis();

    float g = 9.80665f;

    // measured in mG; need to convert to m/s^2
    imuData.linearAcceleration[0] = data->acc_x * g;
    imuData.linearAcceleration[1] = data->acc_y * g;
    imuData.linearAcceleration[2] = data->acc_z * g;

    // measured in deg/s; need to convert to rad/s
    imuData.angularVelocity[0] = (float) MathHelper::deg2rad(data->gyro_x);
    imuData.angularVelocity[1] = (float) MathHelper::deg2rad(data->gyro_y);
    imuData.angularVelocity[2] = (float) MathHelper::deg2rad(data->gyro_z);

    // move into the given ioService to ensure thread safety
    this->ioService->dispatch([this, imuData]()
                              {
                                  this->onImuData(imuData);
                              });
}

void CrazyRadioTransmitter::onCfLog2Data(uint32_t time_in_ms, log2 *data)
{
    if (this->enableLoggingTemperature)
    {
        // measured in degC
        float temperature = data->baro_temp;
        std::cout << "Temperature " << temperature << " °C" << std::endl;
    }

    if (this->enableLoggingMagneticField)
    {
        // measured in Tesla
        cv::Vec3f magneticField;
        magneticField[0] = data->mag_x;
        magneticField[1] = data->mag_y;
        magneticField[2] = data->mag_z;
        std::cout << "Magnetic Field " << magneticField << " Tesla" << std::endl;
    }

    if (this->enableLoggingPressure)
    {
        // hPa (=mbar)
        float pressure = data->baro_pressure;
        std::cout << "Pressure " << pressure << " mbar" << std::endl;
    }

    if (this->enableLoggingBattery)
    {
        // V
        float voltage = data->pm_vbat;
        std::cout << "Voltage " << voltage << " V" << std::endl;
    }
}

void CrazyRadioTransmitter::onLinkQuality(float linkQuality)
{
    if (linkQuality < 0.7)
    {
        std::cout << "Link quality low [ " << linkQuality << " ]." << std::endl;
    }
}

// /////////////////////////////////////////////////////////////////////////////

std::string CrazyRadioTransmitter::getName()
{
    return this->name;
}

bool CrazyRadioTransmitter::isOpen()
{
    return this->running;
}

bool CrazyRadioTransmitter::hasCapacity()
{
    return this->radios.size() < 1;
}

void CrazyRadioTransmitter::addTxModule(AbstractTxModule *radio)
{
    std::string radioURI = radio->getModuleId();

    if (!this->hasCapacity())
    {
        std::string ex = boost::str(boost::format("Cannot add radio with link uri [ %1% ]. Capacity exceeded.") % radioURI);
        BOOST_LOG_TRIVIAL(error) << ex;
        throw RadioException(ex);
    }

    this->radios[radioURI] = radio;
    this->currentRadio = radioURI;
}

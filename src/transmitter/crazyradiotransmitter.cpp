//
// Created by lasse on 29.09.15.
//

#include <opencv2/core/matx.hpp>
#include "crazyradiotransmitter.h"

CrazyRadioTransmitter::CrazyRadioTransmitter(const std::string name,
                                             boost::shared_ptr<boost::asio::io_service> io_service)
{
    this->name = name;
    this->running = false;
    this->currentRadio = -1;
    this->io_service = io_service;
}

void CrazyRadioTransmitter::open()
{
    // check if the thread already created
    if (this->thread)
    {
        return;
    }

    // set running to true
    this->running = true;
    // start the boost thread
    this->thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&CrazyRadioTransmitter::run, this)));
}

void CrazyRadioTransmitter::close()
{

    // set running to false
    this->running = false;

    // check if the thread is avalaibe
    if (this->thread && this->thread->joinable())
    {
        // interrupt the thread
        this->thread->interrupt();
        // wait for the thread has been finished
        this->thread->join();
    }
}

// ###########################################

void CrazyRadioTransmitter::run()
{
    // display info message
    BOOST_LOG_TRIVIAL(info) << "starting crazyradio thread...";
    // initialize the radio
    this->initialize();

    // run until running is false
    while (this->running)
    {
        try
        {
            // update the crazy radio
            this->update();

            // sleep for 10 millis
            boost::posix_time::milliseconds timeout(10);
            boost::this_thread::sleep(timeout);
        } catch (boost::thread_interrupted &)
        {
            BOOST_LOG_TRIVIAL(error) << "the crazy radio thread has been interrupred";
        }
    }

    // clean up the crazy radio
    this->cleanup();

    // display info message
    BOOST_LOG_TRIVIAL(info) << "crazyradio thread stopped";
}

void CrazyRadioTransmitter::initialize()
{
    // the url to open the crazy radio
    std::string crazyRadioUrl = "radio://0/80/250K";

    // create a new radio
    this->radio = boost::shared_ptr<CCrazyRadio>(new CCrazyRadio(crazyRadioUrl));

    // start the radio
    if (this->radio->startRadio())
    {
        // create the copter
        this->copter = boost::shared_ptr<CCrazyflie>(new CCrazyflie(radio.get()));
        // configure the copter
        this->copter->setSendSetpoints(true);
        // set thrust to zero
        this->copter->setThrust(0);
    } else
    {
        // set running to false
        this->running = false;
        // create the exception string
        std::string ex = str(boost::format("fail to open crazy radio with url [ %1% ]") % crazyRadioUrl);
        // display the error
        BOOST_LOG_TRIVIAL(error) << ex;
        // throw an radio exception
        throw RadioException(ex);
    }
}

void CrazyRadioTransmitter::update()
{
    // update the copter radio
    if (this->copter->cycle())
    {
        // flag if request was successful
        bool success = false;
        // the control signals
        cv::Vec4d u(0.0, 0.0, 0.0, 0.0);
        // the latch to wait for the request
        boost::latch latch(2);

        // invoke the radio control request inside the io_service to be thread save
        this->io_service->post([this, &latch, &success, &u]()
                               {
                                   // check if current radio exists
                                   if (this->radios.find(this->currentRadio) != this->radios.end())
                                   {
                                       // get the current radio
                                       AbstractRadio *radio = this->radios[this->currentRadio];
                                       // check if the radio is suspended
                                       if (!radio->isSuspended())
                                       {
                                           // create the Control Values
                                           u = cv::Vec4d(radio->getThrottle(),
                                                         radio->getRoll(),
                                                         radio->getPitch(),
                                                         radio->getYaw());
                                       }
                                       // set success to true
                                       success = true;
                                   }
                                   // release the latch
                                   latch.count_down();
                               });
        // countdown and wait for the latch release
        latch.count_down_and_wait();
        // check if the request was successful
        if (success)
        {
            // update the control signals
            this->copter->setThrust((int) u[0]);
            this->copter->setRoll((float) u[1]);
            this->copter->setPitch((float) u[2]);
            this->copter->setYaw((float) u[3]);
        }

        // publish the copter data from the cflie radio
        this->publishCopterData();
    }
}

void CrazyRadioTransmitter::cleanup()
{
    if (this->copter)
    {
        // free the copter
        this->copter.reset();
        this->copter = NULL;
    }

    if (this->radio)
    {
        // free the radios
        this->radio.reset();
        this->radio = NULL;
    }
}

// ###########################################

void CrazyRadioTransmitter::publishCopterData()
{
    // check if the current radio is available
    if (this->radios.find(this->currentRadio) == this->radios.end())
    {
        return;
    }

    // get the current radio
    AbstractRadio *radio = this->radios[this->currentRadio];

    CopterData data;
    // get the copter id
    data.copterId = radio->getId();

    data.batteryLevel = this->copter->batteryLevel();
    data.batteryState = this->copter->batteryState();

    data.asl = this->copter->asl();
    data.aslLong = this->copter->aslLong();

    data.pressure = this->copter->pressure();
    data.temperature = this->copter->temperature();

    data.accX = this->copter->accX();
    data.accY = this->copter->accY();
    data.accZ = this->copter->accZ();
    data.accZW = this->copter->accZW();

    data.gyroX = this->copter->gyroX();
    data.gyroY = this->copter->gyroY();
    data.gyroZ = this->copter->gyroZ();

    data.magX = this->copter->magX();
    data.magY = this->copter->magY();
    data.magZ = this->copter->magZ();

    // move into the given io_service to be thread safe
    this->io_service->dispatch([this, data](){
        // publish the copter data
        this->onCopterData(data);
    });
}

// ###########################################

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

void CrazyRadioTransmitter::addRadio(AbstractRadio *radio)
{
    // get the transmitter id
    int txId = radio->getTxId();

    if (!this->hasCapacity())
    {
        // create the exception string
        std::string ex = str(
                boost::format("can't add radio with trasmitter id [ %1% ] to vrazy radio transmitter, because no radio capacity available") %
                txId);
        // display the error
        BOOST_LOG_TRIVIAL(error) << ex;
        // throw an radio exception
        throw RadioException(ex);
    }

    // insert the AbstractRadio
    this->radios[txId] = radio;
    // activaed the current radio
    this->currentRadio = txId;

}
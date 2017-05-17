/******************************************************
 *
 *   #, #,         CCCCCC  VV    VV MM      MM RRRRRRR
 *  %  %(  #%%#   CC    CC VV    VV MMM    MMM RR    RR
 *  %    %## #    CC        V    V  MM M  M MM RR    RR
 *   ,%      %    CC        VV  VV  MM  MM  MM RRRRRR
 *   (%      %,   CC    CC   VVVV   MM      MM RR   RR
 *     #%    %*    CCCCCC     VV    MM      MM RR    RR
 *    .%    %/
 *       (%.      Computer Vision & Mixed Reality Group
 *
 *****************************************************/
/** @copyright:   Hochschule RheinMain,
 *                University of Applied Sciences
 *     @author:   Marc Lieser
 *    @version:   1.0
 *       @date:   27.04.2017
 *****************************************************/

#include "rpitx.h"

RpiTX::RpiTX(const std::string name,
             boost::shared_ptr<boost::asio::io_service> io_service) :
        name(name),
        serial(-1),
        initialized(false),
        radio(NULL)
{
    this->io_service = io_service;
    this->timer = new DeadlineTimer((1. / 22.) * 1000., this->io_service);
}

void RpiTX::open()
{
    if ((this->serial = Serial::serialOpen("/dev/serial0", DSM_BAUD_RATE)) < 0)
    {
        std::string ex = "Unable to open serial device.";
        BOOST_LOG_TRIVIAL(error) << ex;
        throw RadioException(ex);
    }

    if (wiringPiSetup() == -1)
    {
        std::string ex = "Unable to start wiringPi.";
        BOOST_LOG_TRIVIAL(error) << ex;
        throw RadioException(ex);
    }

    BOOST_LOG_TRIVIAL(info) << "GPIO serial port initialized.";

    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, LOW);

    this->initialized = true;

    this->timer->start();
    this->timer->invokeTimer(boost::bind(&RpiTX::update, this, boost::asio::placeholders::error));

    BOOST_LOG_TRIVIAL(info) << "Transmission started.";
}

void RpiTX::close()
{
    if (!this->initialized)
    {
        return;
    }

    digitalWrite(POWER_PIN, LOW);
    Serial::serialClose(this->serial);
    this->timer->cancel();
    this->initialized = false;

    BOOST_LOG_TRIVIAL(info) << "GPIO stopped.";
}

std::string RpiTX::getName()
{
    return this->name;
}

bool RpiTX::isOpen()
{
    return this->initialized;
}

bool RpiTX::hasCapacity()
{
    return true;
}

void RpiTX::addRadio(AbstractRadio *radio)
{
    if (this->radio)
    {
        std::string ex = "Raspi Radio already added.";
        BOOST_LOG_TRIVIAL(error) << ex;
        throw RadioException(ex);
    }

    // this is a dsmx radio
    this->radio = static_cast<DSMXRadio *>(radio);

    // add the radio config to our map of configs
    RadioConfig config = this->radio->getRadioConfig();
    int frameRate = config.frameRate;

    BOOST_LOG_TRIVIAL(info) << "DSMX frame rate set to " << frameRate << "ms.";

    // update the timer
    this->timer->updatePeriod(frameRate);
}

void RpiTX::update(const boost::system::error_code &ec)
{
    if (ec)
    {
        std::string ex = boost::str(boost::format("Raspi Radio timer update failed with exception [ %1% ].") % ec.message());
        BOOST_LOG_TRIVIAL(error) << ex;
        throw RadioException(ex);
    }

    if (!this->radio)
    {
        std::string ex = "No radio set.";
        BOOST_LOG_TRIVIAL(error) << ex;
        throw RadioException(ex);
    }

    unsigned char dsmx[DSM_FRAME_LENGTH] = {};

    // prepare array and send signal
    dsmx[header_1] = radio->isBinding() ? header_1_bind_mode : header_1_default;
    dsmx[header_2] = header_2_default;

    int ch1 = ch1_offset + center_value_offset + int(radio->getThrottle() * value_range_scale);
    int ch2 = ch2_offset + center_value_offset + int(radio->getRoll() * value_range_scale);
    int ch3 = ch3_offset + center_value_offset + int(radio->getPitch() * value_range_scale);
    int ch4 = ch4_offset + center_value_offset + int(radio->getYaw() * value_range_scale);
    int ch5 = ch5_offset + center_value_offset + int(radio->getGear() * value_range_scale);
    int ch6 = ch6_offset + center_value_offset + int(radio->getAux1() * value_range_scale);

    // convert to byte frame
    dsmx[channel_1_hi] = SerialHelper::hiByte(ch1);
    dsmx[channel_1_lo] = SerialHelper::loByte(ch1);

    dsmx[channel_2_hi] = SerialHelper::hiByte(ch2);
    dsmx[channel_2_lo] = SerialHelper::loByte(ch2);

    dsmx[channel_3_hi] = SerialHelper::hiByte(ch3);
    dsmx[channel_3_lo] = SerialHelper::loByte(ch3);

    dsmx[channel_4_hi] = SerialHelper::hiByte(ch4);
    dsmx[channel_4_lo] = SerialHelper::loByte(ch4);

    dsmx[channel_5_hi] = SerialHelper::hiByte(ch5);
    dsmx[channel_5_lo] = SerialHelper::loByte(ch5);

    dsmx[channel_6_hi] = SerialHelper::hiByte(ch6);
    dsmx[channel_6_lo] = SerialHelper::loByte(ch6);

    // power
    digitalWrite(POWER_PIN, this->radio->isEnabled() ? HIGH : LOW);

    // signal
    write(this->serial, dsmx, DSM_FRAME_LENGTH);

    // reinvoke timer
    this->timer->invokeTimer(boost::bind(&RpiTX::update, this, boost::asio::placeholders::error));
}

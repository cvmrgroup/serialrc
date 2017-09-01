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
             const std::string serialPortName,
             boost::shared_ptr<boost::asio::io_service> io_service)
        :
        name(name),
        serialPortName(serialPortName),
        serial(-1),
        initialized(false),
        module(NULL)
{
    this->io_service = io_service;
    this->timer = new DeadlineTimer((1. / 22.) * 1000., this->io_service);
}

void RpiTX::open()
{
    if ((this->serial = Serial::serialOpen(this->serialPortName, DSM_BAUD_RATE)) < 0)
    {
        std::string msg = "Unable to open serial device.";
        throw std::runtime_error(msg);
    }

    if (wiringPiSetup() == -1)
    {
        std::string msg = "Unable to start wiringPi.";
        throw std::runtime_error(msg);
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

void RpiTX::addTxModule(AbstractTxModule *txModule)
{
    if (this->module)
    {
        std::string msg = "Raspi Radio already added.";
        throw std::runtime_error(msg);
    }

    // this is a dsmx radio
    this->module = static_cast<DSMXModule *>(txModule);

    // add the radio config to our map of configs
    RadioConfig config = this->module->getRadioConfig();
    int frameRate = config.frameRate;

    BOOST_LOG_TRIVIAL(info) << "DSMX frame rate set to " << frameRate << "ms.";

    // update the timer
    this->timer->updatePeriod(frameRate);
}

void RpiTX::update(const boost::system::error_code &ec)
{
    if (ec)
    {
        std::string msg = boost::str(boost::format("Raspi Radio timer update failed with exception [ %1% ].") % ec.message());
        throw std::runtime_error(msg);
    }

    if (!this->module)
    {
        std::string msg = "No radio set.";
        throw std::runtime_error(msg);
    }

    unsigned char frame[DSM_FRAME_LENGTH] = {};

    DSMXModule *radio = this->module;

    // prepare array and send signal
    frame[header_1] = (unsigned char) radio->isBinding() ? header_1_bind_mode : header_1_default;
    frame[header_2] = (unsigned char) header_2_default;

    std::vector<double> signal = radio->getSignal();

    int ch1 = ch1_offset + center_value_offset + int(signal[AbstractTxModule::ChannelNames::Throttle] * value_range_scale);
    int ch2 = ch2_offset + center_value_offset + int(signal[AbstractTxModule::ChannelNames::Aileron] * value_range_scale);
    int ch3 = ch3_offset + center_value_offset + int(signal[AbstractTxModule::ChannelNames::Elevation] * value_range_scale);
    int ch4 = ch4_offset + center_value_offset + int(signal[AbstractTxModule::ChannelNames::Rudder] * value_range_scale);
    int ch5 = ch5_offset + center_value_offset + int(signal[AbstractTxModule::ChannelNames::Gear] * value_range_scale);
    int ch6 = ch6_offset + center_value_offset + int(signal[AbstractTxModule::ChannelNames::Aux1] * value_range_scale);

    // convert to byte frame
    frame[channel_1_hi] = SerialHelper::hiByte(ch1);
    frame[channel_1_lo] = SerialHelper::loByte(ch1);
    frame[channel_2_hi] = SerialHelper::hiByte(ch2);
    frame[channel_2_lo] = SerialHelper::loByte(ch2);
    frame[channel_3_hi] = SerialHelper::hiByte(ch3);
    frame[channel_3_lo] = SerialHelper::loByte(ch3);
    frame[channel_4_hi] = SerialHelper::hiByte(ch4);
    frame[channel_4_lo] = SerialHelper::loByte(ch4);
    frame[channel_5_hi] = SerialHelper::hiByte(ch5);
    frame[channel_5_lo] = SerialHelper::loByte(ch5);
    frame[channel_6_hi] = SerialHelper::hiByte(ch6);
    frame[channel_6_lo] = SerialHelper::loByte(ch6);

    // power
    digitalWrite(POWER_PIN, this->module->isEnabled() ? HIGH : LOW);

    // signal
    write(this->serial, frame, DSM_FRAME_LENGTH);

    // reinvoke timer
    this->timer->invokeTimer(boost::bind(&RpiTX::update, this, boost::asio::placeholders::error));
}

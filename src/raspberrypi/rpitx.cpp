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
             boost::shared_ptr<boost::asio::io_service> io_service)
{
    this->radio = NULL;
    this->initialized = false;
    this->running = false;
    this->name = name;
    this->io_service = io_service;
}

void RpiTX::open()
{
    this->start();
}

void RpiTX::close()
{
    this->stop();
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
    this->radio = radio;
}

void RpiTX::start()
{
    if (this->thread)
    {
        return;
    }

    this->thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&RpiTX::run, this)));
    this->initialized = true;
}

void RpiTX::run()
{
    BOOST_LOG_TRIVIAL(info) << "Starting GPIO thread...";

    if (!this->radio)
    {
        BOOST_LOG_TRIVIAL(error) << "No radio set.";
        return;
    }

    unsigned char dsmx[DSM_FRAME_LENGTH] = {0};

    int fd;

    if ((fd = serialOpen("/dev/serial0", DSM_BAUD_RATE)) < 0)
    {
        BOOST_LOG_TRIVIAL(error) << "Unable to open serial device [ " << strerror(errno) << " ].";
        return;
    }

    if (wiringPiSetup() == -1)
    {
        BOOST_LOG_TRIVIAL(error) << "Unable to start wiringPi [ " << strerror(errno) << " ].";
        return;
    }

    this->running = true;

    pinMode(DARLINGTON_PIN, OUTPUT);
    pinMode(DSM_SIGNAL_PIN, OUTPUT);

    unsigned int nextTime = millis() + DSM_SEND_RATE;

    while (this->running)
    {
        bool enabled = this->radio->isEnabled();
        digitalWrite(DARLINGTON_PIN, enabled ? HIGH : LOW);

        if (millis() > nextTime)
        {
            // prepare array and send signal
            dsmx[header_1] = radio->isBinding() ? header_1_bind_mode : header_1_default;
            dsmx[header_2] = header_2_default;

            int ch1 = ch1_offset + center_value_offset + int(radio->getThrottle() * value_range_scale);
            int ch2 = ch2_offset + center_value_offset + int(radio->getRoll() * value_range_scale);
            int ch3 = ch3_offset + center_value_offset + int(radio->getPitch() * value_range_scale);
            int ch4 = ch4_offset + center_value_offset + int(radio->getYaw() * value_range_scale);
            int ch5 = ch5_offset + center_value_offset + int(radio->getCh5() * value_range_scale);
            int ch6 = ch6_offset + center_value_offset + int(radio->getCh6() * value_range_scale);

            // overwrite throttle signal if copter is supended
            if (radio->isSuspended())
            {
                ch1 = ch1_offset + center_value_offset + int(-1.0 * value_range_scale);
            }

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

            write(fd, dsmx, DSM_FRAME_LENGTH);

            // set next send time
            nextTime += DSM_SEND_RATE;
        }
    }

    BOOST_LOG_TRIVIAL(info) << "GPIO thread stopped.";
}

void RpiTX::stop()
{
    this->running = false;
}

void RpiTX::join()
{
    if (this->thread && this->thread->joinable())
    {
        this->thread->join();
    }
}

bool RpiTX::isRunning()
{
    return this->running;
}

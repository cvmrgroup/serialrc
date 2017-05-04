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

    int fd;

    if ((fd = Serial::serialOpen("/dev/serial0", DSM_BAUD_RATE)) < 0)
    {
        BOOST_LOG_TRIVIAL(error) << "Unable to open serial device [ " << errno << " ].";
        return;
    }

    if (wiringPiSetup() == -1)
    {
        BOOST_LOG_TRIVIAL(error) << "Unable to start wiringPi [ " << errno << " ].";
        return;
    }

    this->running = true;
    
    pinMode(DARLINGTON_PIN, OUTPUT);
    digitalWrite(DARLINGTON_PIN, LOW);
    
    int ch1, ch2, ch3, ch4, ch5, ch6;
    unsigned char dsmx[DSM_FRAME_LENGTH] = {};
    
    bool binding = false;

    unsigned int offTime = 0;
    unsigned int bindTime = 0;

    unsigned int nextTime =  millis() + DSM_SEND_RATE;

    while (this->running)
    {
	if (radio->isBinding() && !binding)
	{
	    binding = true;
	    offTime = millis() + 1000;
	    bindTime = millis() + 5000;
	    
	    digitalWrite(DARLINGTON_PIN, LOW);
	    dsmx[header_1] = header_1_bind_mode;

	    BOOST_LOG_TRIVIAL(info) << "start binding";
	}

	if (binding)
	{
	    if (millis() > bindTime)
	    {
		dsmx[header_1] = header_1_default;
		BOOST_LOG_TRIVIAL(info) << "bind time over";

		binding = false;
	    }
	    else if (millis() > offTime)
	    {
		digitalWrite(DARLINGTON_PIN, HIGH);
		BOOST_LOG_TRIVIAL(info) << "turn on";
	    }
	}
	
	// make sure to switch on tx with bind signal applied
	//bool enabled = this->radio->isEnabled();
	//digitalWrite(DARLINGTON_PIN, enabled ? HIGH : LOW);
	
	// prepare array and send signal
	//dsmx[header_1] = radio->isBinding() ? header_1_bind_mode : header_1_default;
	//dsmx[header_2] = header_2_default;
	
	ch1 = ch1_offset + center_value_offset + int(radio->getThrottle() * value_range_scale);
	ch2 = ch2_offset + center_value_offset + int(radio->getRoll() * value_range_scale);
	ch3 = ch3_offset + center_value_offset + int(radio->getPitch() * value_range_scale);
	ch4 = ch4_offset + center_value_offset + int(radio->getYaw() * value_range_scale);
	ch5 = ch5_offset + center_value_offset + int(radio->getCh5() * value_range_scale);
	ch6 = ch6_offset + center_value_offset + int(radio->getCh6() * value_range_scale);
	
	// overwrite throttle signal if copter is supended
	if (radio->isSuspended())
	{
	    ch1 = ch1_offset + center_value_offset + int(-1.0 * value_range_scale);
	}

	if (millis() > nextTime)
	{
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

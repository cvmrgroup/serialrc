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

    this->running = true;

    /*wiringPiSetup();
    pinMode(0, OUTPUT);
    
    while(this->running)
    {
	digitalWrite(0, HIGH);
	delay(500);
	digitalWrite(0, LOW);
	delay(500);*/

    while (this->running)
    {

    }

    /*
    try
    {
        this->io_service->run();
    }
    catch (std::exception &e)
    {
        BOOST_LOG_TRIVIAL(error) << "Exception: " << e.what();
    }
    */

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

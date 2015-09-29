//
// Created by lasse on 29.09.15.
//

#include "crazyradiotransmitter.h"

CrazyRadioTransmitter::CrazyRadioTransmitter(const std::string name,
                                             boost::shared_ptr<boost::asio::io_service> io_service)
{
    this->name = name;
    this->running = false;
    this->currentRadio = 0;
    this->io_service = io_service;
    this->timer = boost::shared_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(*io_service));
}

void CrazyRadioTransmitter::open()
{
    // the url to open the crazy radio
    std::string crazyRadioUrl = "radio://0/80/250K";

    // create a new radio
    this->radio = boost::shared_ptr<CCrazyRadio>(new CCrazyRadio(crazyRadioUrl));

    if (this->radio->startRadio())
    {
        // create the copter
        this->copter = boost::shared_ptr<CCrazyflie>(new CCrazyflie(radio.get()));
        // configure the copter
        this->copter->setSendSetpoints(true);
        this->copter->setThrust(0);

        // set running to true
        this->running = true;

        // start the crazy radio update loop
        this->invokeTimer();
    } else
    {
        // create the exception string
        std::string ex = str(boost::format("fail to open crazy radio with url [ %1% ]") % crazyRadioUrl);
        // display the error
        BOOST_LOG_TRIVIAL(error) << ex;
        // throw an radio exception
        throw RadioException(ex);
    }
}

void CrazyRadioTransmitter::close()
{
    // set running to false
    this->running = false;
}

// ###########################################


void CrazyRadioTransmitter::invokeTimer()
{
    // get a delay from 10 ms
    boost::posix_time::milliseconds timeout(10);
    this->timer->expires_from_now(timeout);
    // add the async callback for the timer
    this->timer->async_wait(boost::bind(&CrazyRadioTransmitter::update, this, boost::asio::placeholders::error));
}

void CrazyRadioTransmitter::update(const boost::system::error_code &ec)
{
    // check if an error happens
    if (ec)
    {
        std::string ex = str(boost::format("fail to update crazy radio transmitter, because [ %1% ]") % ec.message());
        // display error
        BOOST_LOG_TRIVIAL(error) << ex;
        // throw the exception
        throw RadioException(ex);
    }

    // check if the CrazyRadioTransmitter is running
    if (!this->running)
    {
        return;
    }

    if (this->copter->cycle())
    {
        // check if the current radio avalable
        if (this->radios.find(this->currentRadio) != this->radios.end())
        {
            // get the current radio
            AbstractRadio *radio = this->radios[this->currentRadio];

            int throttle = (int) radio->getThrottle();
            float roll = (float) radio->getRoll();
            float pitch = (float) radio->getPitch();
            float yaw = (float) radio->getYaw();

            // check if suspended given
            if (radio->isSuspended())
            {
                throttle = 0;
                roll = pitch = yaw = 0.0;
            }

            this->copter->setThrust(throttle);
            this->copter->setRoll(roll);
            this->copter->setPitch(pitch);
            this->copter->setYaw(yaw);
        }
    }

    // restart the timer for next update
    this->invokeTimer();
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
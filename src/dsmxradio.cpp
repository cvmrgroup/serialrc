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
 *       @date:   13.05.2014
 *****************************************************/

#include "dsmxradio.h"

DSMXRadio::DSMXRadio(int id, RadioConfig config) :
        AbstractRadio(id, config.txId)
{
    this->config = config;

    // default values
    this->throttle = config.channels[RadioConfig::Channel::Throttle].travelMin;
    this->roll = 0.0;
    this->pitch = 0.0;
    this->yaw = 0.0;
    this->ch5 = 0.0;
    this->ch6 = 0.0;
}

DSMXRadio::~DSMXRadio()
{
}

RadioConfig DSMXRadio::getRadioConfig()
{
    return this->config;
}

void DSMXRadio::toggleSender()
{
    this->enabled = !this->enabled;
}

void DSMXRadio::turnSenderOn()
{
    this->enabled = true;
}

void DSMXRadio::turnSenderOff()
{
    this->enabled = false;
}

void DSMXRadio::suspend(bool suspended)
{
    this->suspended = suspended;
}

void DSMXRadio::toggleSuspension()
{
    this->suspended = !this->suspended;
}

void DSMXRadio::setBindSignal()
{
    this->enabled = true;
    this->binding = true;
}

void DSMXRadio::setControls(double throttle, double roll, double pitch,
                            double yaw)
{
    this->binding = false;

    this->throttle = throttle;
    this->roll = -roll;
    this->pitch = pitch;
    this->yaw = yaw;
}

void DSMXRadio::toggleCh5()
{
    this->ch5 *= -1.0;
}

void DSMXRadio::toggleCh6()
{
    this->ch6 *= -1.0;
}

void DSMXRadio::setArmSignal()
{
    this->throttle = -1.0;
    this->yaw = 1.0;
}

void DSMXRadio::setDisarmSignal()
{
    this->throttle = -1.0;
    this->yaw = -1.0;
}

void DSMXRadio::emergencyStop(bool emergency)
{
    suspend(true);
}

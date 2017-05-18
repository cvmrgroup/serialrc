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
#include <iostream>

DSMXRadio::DSMXRadio(int id, RadioConfig &config) :
        AbstractRadio(id, config.txId)
{
    this->config = config;
    this->setSuspensionSignal();
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

    if (this->suspended)
    {
        this->setSuspensionSignal();
    }
}

void DSMXRadio::toggleSuspension()
{
    this->suspended = !this->suspended;

    if (this->suspended)
    {
        this->setSuspensionSignal();
    }
}

void DSMXRadio::setBindSignal()
{
    this->enabled = true;
    this->binding = true;
}

void DSMXRadio::setControls(double throttle, double roll, double pitch,
                            double yaw)
{
    if (this->suspended)
    {
        // do not accept new control inputs if copter is suspended
        return;
    }

    this->binding = false;

    this->signal[Throttle] = this->config.channels[Throttle].getServoTravel(throttle);
    this->signal[Aileron] = this->config.channels[Aileron].getServoTravel(-roll);
    this->signal[Elevation] = this->config.channels[Elevation].getServoTravel(pitch);
    this->signal[Rudder] = this->config.channels[Rudder].getServoTravel(yaw);
}

void DSMXRadio::toggleGear()
{
    this->signal[Gear] = this->config.channels[Gear].nextPosition();
}

void DSMXRadio::toggleAux1()
{
    this->signal[Aux1] = this->config.channels[Aux1].nextPosition();
}

void DSMXRadio::setSuspensionSignal()
{
    for (int i = 0; i < N_CHANNELS; i++)
    {
        this->signal[i] = 0.0;
    }

    this->signal[Throttle] = -config.channels[Throttle].travel;
}

void DSMXRadio::setArmSignal()
{
    std::copy(std::begin(this->config.armSignal), std::end(this->config.armSignal), std::begin(this->signal));
}

void DSMXRadio::setDisarmSignal()
{
    std::copy(std::begin(this->config.disarmSignal), std::end(this->config.disarmSignal), std::begin(this->signal));
}

void DSMXRadio::emergencyStop(bool emergency)
{
    suspend(true);
}

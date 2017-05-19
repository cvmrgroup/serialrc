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

#include "dsmxmodule.h"

DSMXModule::DSMXModule(int copterId, RadioConfig &config) :
        AbstractTxModule(copterId, config.transmitter, config.txId)
{
    this->config = config;
    this->setSuspensionSignal();
    this->signal[Gear] = this->config.channels[Gear].getSwitchValue();
    this->signal[Aux1] = this->config.channels[Aux1].getSwitchValue();
}

DSMXModule::~DSMXModule()
{
}

RadioConfig DSMXModule::getRadioConfig()
{
    return this->config;
}

void DSMXModule::toggleSender()
{
    this->enabled = !this->enabled;
}

void DSMXModule::turnSenderOn()
{
    this->enabled = true;
}

void DSMXModule::turnSenderOff()
{
    this->enabled = false;
}

void DSMXModule::suspend(bool suspended)
{
    this->suspended = suspended;

    if (this->suspended)
    {
        this->setSuspensionSignal();
    }
}

void DSMXModule::toggleSuspension()
{
    this->suspended = !this->suspended;

    if (this->suspended)
    {
        this->setSuspensionSignal();
    }
}

void DSMXModule::setBindSignal()
{
    this->enabled = true;
    this->binding = true;
}

void DSMXModule::setControls(double throttle, double roll, double pitch,
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

void DSMXModule::toggleGear()
{
    this->signal[Gear] = this->config.channels[Gear].nextPosition();
}

void DSMXModule::toggleAux1()
{
    this->signal[Aux1] = this->config.channels[Aux1].nextPosition();
}

void DSMXModule::setSuspensionSignal()
{
    for (int i = 1; i < 4; i++)
    {
        this->signal[i] = 0.0;
    }

    this->signal[Throttle] = -config.channels[Throttle].travel;
}

void DSMXModule::setArmSignal()
{
    std::copy(std::begin(this->config.armSignal), std::end(this->config.armSignal), std::begin(this->signal));
}

void DSMXModule::setDisarmSignal()
{
    std::copy(std::begin(this->config.disarmSignal), std::end(this->config.disarmSignal), std::begin(this->signal));
}

void DSMXModule::emergencyStop(bool emergency)
{
    suspend(true);
}

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

DSMXModule::DSMXModule(int copterId, TxModuleConfig &config)
        : AbstractTxModule(copterId, config.transmitter, config.txId)
{
    this->config = config;
    this->enabled = false;
    this->binding = false;

    // add two channels
    for (int i = 0; i < 2; i++)
    {
        this->signal.push_back(0.);
    }

    // initialize signal
    this->signal[Throttle] = -config.channels[Throttle].travel;
    this->signal[Aileron] = 0.;
    this->signal[Elevation] = 0.;
    this->signal[Rudder] = 0.;
    this->signal[Gear] = this->config.channels[Gear].getSwitchValue();
    this->signal[Aux1] = this->config.channels[Aux1].getSwitchValue();

    // disarm
    this->setDisarmSignal();
}

TxModuleConfig DSMXModule::getConfig()
{
    return this->config;
}

void DSMXModule::toggleSender()
{
    this->enabled = !this->enabled;
}

void DSMXModule::suspend(bool suspended)
{
    this->suspended = suspended;

    if (this->suspended)
    {
        this->signal[Throttle] = -config.channels[Throttle].travel;
        this->setDisarmSignal(); // if provided
    }
    else
    {
        this->setArmSignal(); // if provided
    }
}

void DSMXModule::toggleSuspension()
{
    this->suspended = !this->suspended;
    this->suspend(this->suspended);
}

void DSMXModule::setBindSignal()
{
    this->enabled = true;
    this->binding = true;
}

void DSMXModule::setControls(ControlInput u)
{
    if (this->suspended)
    {
        // do not accept new control inputs if copter is suspended
        return;
    }

    this->binding = false;

    this->signal[Throttle] = this->config.channels[Throttle].getServoTravel(u.thrust);
    this->signal[Aileron] = this->config.channels[Aileron].getServoTravel(u.roll);
    this->signal[Elevation] = this->config.channels[Elevation].getServoTravel(u.pitch);
    this->signal[Rudder] = this->config.channels[Rudder].getServoTravel(u.yaw);
}

void DSMXModule::toggleGear()
{
    this->signal[Gear] = this->config.channels[Gear].nextPosition();
}

void DSMXModule::setGear(bool state)
{
    this->signal[Gear] = this->config.channels[Gear].getServoTravel(state ? 1. : -1.);
}

void DSMXModule::toggleAux1()
{
    this->signal[Aux1] = this->config.channels[Aux1].nextPosition();
}

void DSMXModule::setArmSignal()
{
    if (this->config.armSignalProvided)
    {
        for (int i = 0; i < this->signal.size(); i++)
        {
            if (this->config.armSignalMap.find(i) != this->config.armSignalMap.end())
            {
                double lever = this->config.armSignalMap[i];
                this->signal[i] = this->config.channels[i].getServoTravel(lever);
            }
        }
    }

    // sync with suspension
    this->suspended = false;
}

void DSMXModule::setDisarmSignal()
{
    if (this->config.disarmSignalProvided)
    {
        for (int i = 0; i < 6; i++)
        {
            if (this->config.disarmSignalMap.find(i) != this->config.disarmSignalMap.end())
            {
                double lever = this->config.disarmSignalMap[i];
                this->signal[i] = this->config.channels[i].getServoTravel(lever);
            }
        }
    }

    // sync with suspension
    this->suspended = true;
}

bool DSMXModule::isEnabled()
{
    return this->enabled;
}

bool DSMXModule::isBinding()
{
    return this->binding;
}
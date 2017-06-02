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

    this->signal[Throttle] = -config.channels[Throttle].travel;
    this->signal[Aileron] = 0.;
    this->signal[Elevation] = 0.;
    this->signal[Rudder] = 0.;
    this->signal[Gear] = this->config.channels[Gear].getSwitchValue();
    this->signal[Aux1] = this->config.channels[Aux1].getSwitchValue();

    this->setDisarmSignal();
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

void DSMXModule::setArmSignal()
{
    if (this->config.armSignalProvided)
    {
        for (int i = 0; i < 6; i++)
        {
            if (this->config.armSignalMask[i])
            {
                this->signal[i] = this->config.channels[i].getServoTravel(this->config.armSignal[i]);
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
            if (this->config.disarmSignalMask[i])
            {
                this->signal[i] = this->config.channels[i].getServoTravel(this->config.disarmSignal[i]);
            }
        }
    }

    // sync with suspension
    this->suspended = true;
}

void DSMXModule::emergencyStop(bool emergency)
{
    suspend(true);
}

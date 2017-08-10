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
 *       @date:   24.06.2015
 *****************************************************/

#include "crazyradiomodule.h"

CrazyRadioModule::CrazyRadioModule(int id, std::string sender, std::string txId)
        :
        AbstractTxModule(id, sender, txId)
{
}

CrazyRadioModule::~CrazyRadioModule()
{
}

void CrazyRadioModule::setControls(double throttle, double roll, double pitch,
                                   double yaw)
{
    if (this->suspended)
    {
        return;
    }

    int throttle_center_value_offset = 30000;
    int throttle_value_range_scale = 30000;

    this->signal[Throttle] = (throttle_center_value_offset + throttle * throttle_value_range_scale);
    this->signal[Aileron] = (roll * 45);
    this->signal[Elevation] = (pitch * 45);
    this->signal[Rudder] = (yaw * -180);
}

void CrazyRadioModule::suspend(bool suspended)
{
    this->suspended = suspended;

    if (this->suspended)
    {
        this->setSuspensionSignal();
    }
}

void CrazyRadioModule::toggleSuspension()
{
    this->suspended = !this->suspended;

    if (this->suspended)
    {
        this->setSuspensionSignal();
    }
}

void CrazyRadioModule::emergencyStop()
{
    suspend(true);
}

void CrazyRadioModule::setSuspensionSignal()
{
    for (int i = 0; i < N_CHANNELS; i++)
    {
        this->signal[i] = 0;
    }
}

// not implemented /////////////////////////////////////////////////////////////

void CrazyRadioModule::toggleSender()
{
}

void CrazyRadioModule::turnSenderOn()
{
}

void CrazyRadioModule::turnSenderOff()
{
}

void CrazyRadioModule::setBindSignal()
{
}

void CrazyRadioModule::toggleGear()
{
}

void CrazyRadioModule::setGear(bool state)
{
}

void CrazyRadioModule::toggleAux1()
{
}
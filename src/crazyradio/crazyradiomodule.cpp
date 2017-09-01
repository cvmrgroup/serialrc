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

CrazyRadioModule::CrazyRadioModule(int copterId, std::string sender, int txId, std::string radioUri)
        : AbstractTxModule(copterId, sender, txId),
          radioUri(radioUri)
{
}

std::string CrazyRadioModule::getRadioUri()
{
    return this->radioUri;
}

void CrazyRadioModule::setControls(ControlInput u)
{
    if (this->suspended)
    {
        return;
    }

    int throttle_center_value_offset = 30000;
    int throttle_value_range_scale = 30000;

    this->signal[Throttle] = (throttle_center_value_offset + u.thrust * throttle_value_range_scale);
    this->signal[Aileron] = (-u.roll * 45);
    this->signal[Elevation] = (-u.pitch * 45);
    this->signal[Rudder] = (u.yaw * -180);
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

void CrazyRadioModule::setSuspensionSignal()
{
    for (int i = 0; i < this->nChannels; i++)
    {
        this->signal[i] = 0.;
    }
}

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

#include "crazyradio.h"

CrazyRadio::CrazyRadio(int id, std::string txId) :
        AbstractRadio(id, txId)
{
}

CrazyRadio::~CrazyRadio()
{
}

void CrazyRadio::setControls(double throttle, double roll, double pitch,
                             double yaw)
{
    int throttle_center_value_offset = 30000;
    int throttle_value_range_scale = 30000;

    this->signal[Throttle] = (throttle_center_value_offset + throttle * throttle_value_range_scale);
    this->signal[Aileron] = (roll * 45);
    this->signal[Elevation] = (pitch * 45);
    this->signal[Rudder] = (yaw * -180);
}

void CrazyRadio::suspend(bool suspended)
{
    this->suspended = suspended;
}

void CrazyRadio::toggleSuspension()
{
    this->suspended = !this->suspended;
}

void CrazyRadio::emergencyStop(bool emergency)
{
    suspend(true);
}

// not implemented /////////////////////////////////////////////////////////////

void CrazyRadio::toggleSender()
{
}

void CrazyRadio::turnSenderOn()
{
}

void CrazyRadio::turnSenderOff()
{
}

void CrazyRadio::setBindSignal()
{
}

void CrazyRadio::setArmSignal()
{
}

void CrazyRadio::setDisarmSignal()
{
}

void CrazyRadio::toggleGear()
{
}

void CrazyRadio::toggleAux1()
{
}
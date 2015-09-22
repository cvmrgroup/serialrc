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

using namespace std;

CrazyRadio::CrazyRadio(unsigned int id, unsigned int txId, std::string identifier) :
AbstractRadio(id, txId, identifier)
{
    throttle = 0;
    roll = 0;
    pitch = 0;
    yaw = 0;
}

CrazyRadio::~CrazyRadio()
{
}

void CrazyRadio::setControls(double _throttle, double _roll, double _pitch, double _yaw)
{
    int throttle_center_value_offset = 30000;
    int throttle_value_range_scale = 60000;
    
    throttle = (int)(throttle_center_value_offset + _throttle * throttle_value_range_scale);
    roll = (float)(_roll*-45);
    pitch = (float)(_pitch*45);
    yaw = (float)(_yaw*-180);
}

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

void CrazyRadio::suspend(bool _suspended)
{
    suspended = _suspended;
}

void CrazyRadio::toggleSuspension()
{
    suspended = !suspended;
}

void CrazyRadio::setArmSignal()
{
}

void CrazyRadio::setDisarmSignal()
{
}

void CrazyRadio::toggleCh5()
{
}

void CrazyRadio::toggleCh6()
{
}

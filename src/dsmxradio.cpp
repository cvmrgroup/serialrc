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

DSMXRadio::DSMXRadio(unsigned int id, unsigned int txId, std::string identifier) :
AbstractRadio(id, txId, identifier)
{
    // default values
    throttle = -1.0;
    roll =  0.0;
    pitch =  0.0;
    yaw =  0.0;
    ch5 =  1.0; // -1.0 for copters other than nano 3d
    ch6 = -1.0; // todo 1.0 for initial horizon mode when using superx
}

DSMXRadio::~DSMXRadio()
{
}

void DSMXRadio::toggleSender()
{
    enabled = !enabled;
}

void DSMXRadio::turnSenderOn()
{
    enabled = true;
}

void DSMXRadio::turnSenderOff()
{
    enabled = false;
}

void DSMXRadio::suspend(bool _suspended)
{
    suspended = _suspended;
}

void DSMXRadio::toggleSuspension()
{
    suspended = !suspended;
}

void DSMXRadio::setBindSignal()
{
    binding = true;
    enabled = true;
}

void DSMXRadio::setControls(double _throttle, double _roll, double _pitch, double _yaw)
{
    binding = false;
    
    throttle = _throttle;
    roll = _roll;
    pitch = _pitch;
    yaw = _yaw;
}

void DSMXRadio::toggleCh5()
{
    ch5 *= -1.0;
}

void DSMXRadio::toggleCh6()
{
    ch6 *= -1.0;
}

void DSMXRadio::setArmSignal()
{
    throttle = -1.0;
    yaw =  1.0;
}

void DSMXRadio::setDisarmSignal()
{
    throttle = -1.0;
    yaw = -1.0;
}
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
 *       @date:   16.02.2015
 *****************************************************/

#include "abstractradio.h"

AbstractRadio::AbstractRadio(int _id, std::string _txId)
{
    id = _id;
    txId = _txId;

    binding = false;
    enabled = false;
    suspended = true;
    emergency = false;
}

AbstractRadio::~AbstractRadio()
{
}

int AbstractRadio::getId()
{
    return id;
}

std::string AbstractRadio::getTxId()
{
    return txId;
}

bool AbstractRadio::isEnabled()
{
    return enabled;
}

bool AbstractRadio::isBinding()
{
    return binding;
}

bool AbstractRadio::isSuspended()
{
    return suspended;
}

double AbstractRadio::getThrottle()
{
    return throttle;
}

double AbstractRadio::getRoll()
{
    return roll;
}

double AbstractRadio::getPitch()
{
    return pitch;
}

double AbstractRadio::getYaw()
{
    return yaw;
}

double AbstractRadio::getCh5()
{
    return ch5;
}

double AbstractRadio::getCh6()
{
    return ch6;
}

bool AbstractRadio::isEmergency()
{
    return emergency;
}

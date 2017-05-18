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

AbstractRadio::AbstractRadio(int id, std::string sender, std::string txId) :
        id(id), sender(sender), txId(txId)
{
    binding = false;
    enabled = false;
    suspended = true;
    emergency = false;

    for (int i = 0; i < N_CHANNELS; i++)
    {
        this->signal[i] = 0;
    }
}

AbstractRadio::~AbstractRadio()
{
}

int AbstractRadio::getId()
{
    return id;
}

std::string AbstractRadio::getSender()
{
    return sender;
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
    return this->signal[Throttle];
}

double AbstractRadio::getRoll()
{
    return this->signal[Aileron];
}

double AbstractRadio::getPitch()
{
    return this->signal[Elevation];
}

double AbstractRadio::getYaw()
{
    return this->signal[Rudder];
}

double AbstractRadio::getGear()
{
    return this->signal[Gear];
}

double AbstractRadio::getAux1()
{
    return this->signal[Aux1];
}

bool AbstractRadio::isEmergency()
{
    return emergency;
}

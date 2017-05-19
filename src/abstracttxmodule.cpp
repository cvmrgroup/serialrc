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

#include "abstracttxmodule.h"

AbstractTxModule::AbstractTxModule(int copterId, std::string txName,
                                   std::string moduleId) :
        copterId(copterId), txName(txName), moduleId(moduleId)
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

AbstractTxModule::~AbstractTxModule()
{
}

int AbstractTxModule::getCopterId()
{
    return copterId;
}

std::string AbstractTxModule::getTxName()
{
    return txName;
}

std::string AbstractTxModule::getModuleId()
{
    return moduleId;
}

bool AbstractTxModule::isEnabled()
{
    return enabled;
}

bool AbstractTxModule::isBinding()
{
    return binding;
}

bool AbstractTxModule::isSuspended()
{
    return suspended;
}

double AbstractTxModule::getThrottle()
{
    return this->signal[Throttle];
}

double AbstractTxModule::getRoll()
{
    return this->signal[Aileron];
}

double AbstractTxModule::getPitch()
{
    return this->signal[Elevation];
}

double AbstractTxModule::getYaw()
{
    return this->signal[Rudder];
}

double AbstractTxModule::getGear()
{
    return this->signal[Gear];
}

double AbstractTxModule::getAux1()
{
    return this->signal[Aux1];
}

bool AbstractTxModule::isEmergency()
{
    return emergency;
}

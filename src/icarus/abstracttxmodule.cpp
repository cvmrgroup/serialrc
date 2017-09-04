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

AbstractTxModule::AbstractTxModule(int copterId, std::string txName, int moduleId)
        : copterId(copterId),
          txName(txName),
          moduleId(moduleId),
          nChannels(4),
          suspended(true)
{
    for (int i = 0; i < this->nChannels; i++)
    {
        this->signal.push_back(0.);
    }
}

int AbstractTxModule::getCopterId()
{
    return this->copterId;
}

std::string AbstractTxModule::getTxName()
{
    return this->txName;
}

int AbstractTxModule::getModuleId()
{
    return this->moduleId;
}

std::vector<double> AbstractTxModule::getSignal()
{
    return this->signal;
}

int AbstractTxModule::getNumberOfChannels()
{
    return this->nChannels;
}

bool AbstractTxModule::isSuspended()
{
    return this->suspended;
}

// /////////////////////////////////////////////////////////////////////////////

bool AbstractTxModule::isEnabled()
{
    return false;
}

bool AbstractTxModule::isBinding()
{
    return false;
}

void AbstractTxModule::toggleSender()
{
}

void AbstractTxModule::setBindSignal()
{
}

void AbstractTxModule::toggleGear()
{
}

void AbstractTxModule::setGear(bool state)
{
}

void AbstractTxModule::toggleAux1()
{
}

// /////////////////////////////////////////////////////////////////////////////

void AbstractTxModule::reset()
{
}

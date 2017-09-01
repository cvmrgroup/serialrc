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
/** @copyright    Hochschule RheinMain,
 *                University of Applied Sciences
 *     @author    Marc Lieser
 *    @version    1.0
 *       @date    20.11.2016
 *****************************************************/

#include "dyscotx.h"

DyscoTx::DyscoTx(std::string name)
        : name(name)
{
}

void DyscoTx::addTxModule(AbstractTxModule *module)
{
    int txId = module->getModuleId();
    this->modules[txId] = module;
}

void DyscoTx::open()
{
}

void DyscoTx::close()
{
}

std::string DyscoTx::getName()
{
    return this->name;
}

bool DyscoTx::isOpen()
{
    return true;
}

bool DyscoTx::hasCapacity()
{
    return true;
}



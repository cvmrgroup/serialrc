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
 *       @date    27.06.2017
*****************************************************/

#include <boost/log/trivial.hpp>
#include "dyscomodule.h"

DyscoModule::DyscoModule(int copterId, std::string txName, std::string moduleId)
        :
        AbstractTxModule(copterId, txName, moduleId)
{
}

DyscoModule::~DyscoModule()
{
}

void DyscoModule::setControls(double throttle, double roll, double pitch,
                              double yaw)
{
    if (this->onControlCommand.empty())
    {
        BOOST_LOG_TRIVIAL(info) << "onControlCommand callback empty.";
        return;
    }

    this->onControlCommand(this->copterId, cv::Vec4d(throttle, roll, pitch, yaw));
}

void DyscoModule::toggleGear()
{
    if (this->onHorizonCommand.empty())
    {
        BOOST_LOG_TRIVIAL(info) << "onHorizonCommand callback empty.";
        return;
    }

    this->onHorizonCommand(this->copterId);
}

void DyscoModule::toggleAux1()
{
    if (this->onResetCommand.empty())
    {
        BOOST_LOG_TRIVIAL(info) << "onResetCommand callback empty.";
        return;
    }

    this->onResetCommand(this->copterId);
}

// not implemented /////////////////////////////////////////////////////////////

void DyscoModule::suspend(bool suspended)
{
}

void DyscoModule::toggleSuspension()
{
}

void DyscoModule::emergencyStop(bool emergency)
{
}

void DyscoModule::setSuspensionSignal()
{
}

void DyscoModule::toggleSender()
{
}

void DyscoModule::turnSenderOn()
{
}

void DyscoModule::turnSenderOff()
{
}

void DyscoModule::setBindSignal()
{
}

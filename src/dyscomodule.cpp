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

#include "dyscomodule.h"

DyscoModule::DyscoModule(int copterId, std::string txName, std::string moduleId)
        :
        AbstractTxModule(copterId, txName, moduleId)
{
}

void DyscoModule::setControls(double throttle, double roll, double pitch,
                              double yaw)
{
    // set control values
    this->signal[Throttle] = throttle;
    this->signal[Aileron] = roll;
    this->signal[Elevation] = pitch;
    this->signal[Rudder] = yaw;

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

    this->horizon = !this->horizon;

    this->onHorizonCommand(this->copterId, this->horizon);
}

void DyscoModule::setGear(bool state)
{
    if (this->onHorizonCommand.empty())
    {
        BOOST_LOG_TRIVIAL(info) << "onHorizonCommand callback empty.";
        return;
    }

    this->horizon = state;

    this->onHorizonCommand(this->copterId, this->horizon);
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

void DyscoModule::emergencyStop()
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

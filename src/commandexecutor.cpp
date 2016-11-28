//
// Created by lasse on 29.09.15.
//

#include "commandexecutor.h"

CommandExecutor::CommandExecutor()
{
    this->initializeBindings();
}

void CommandExecutor::initializeBindings()
{
    this->bindings[RadioCommandType::ARM] = boost::bind(&CommandExecutor::onArmCommand, this, _1, _2);
    this->bindings[RadioCommandType::BIND_TRANSMITTER] = boost::bind(&CommandExecutor::onBindTransmitterCommand, this, _1, _2);
    this->bindings[RadioCommandType::CONTROL] = boost::bind(&CommandExecutor::onControlCommand, this, _1, _2);
    this->bindings[RadioCommandType::DISARM] = boost::bind(&CommandExecutor::onDisarmCommand, this, _1, _2);
    this->bindings[RadioCommandType::EMERGENCY] = boost::bind(&CommandExecutor::onEmergencyCommand, this, _1, _2);
    this->bindings[RadioCommandType::RESUME_COPTER] = boost::bind(&CommandExecutor::onResumeCopterCommand, this, _1, _2);
    this->bindings[RadioCommandType::SUSPEND_COPTER] = boost::bind(&CommandExecutor::onSuspendCopterCommand, this, _1, _2);
    this->bindings[RadioCommandType::TAKE_OVER] = boost::bind(&CommandExecutor::onTakeOverCommand, this, _1, _2);
    this->bindings[RadioCommandType::TOGGLE_SUSPEND_COPTER] = boost::bind(&CommandExecutor::onToggleSuspensionCommand, this, _1, _2);
    this->bindings[RadioCommandType::TOGGLE_TRANSMITTER] = boost::bind(&CommandExecutor::onToggleTransmitterCommand, this, _1, _2);
}

// /////////////////////////////////////////////////////////////////////////////

void CommandExecutor::execute(IRadioCommand *command, AbstractRadio *radio)
{
    // get the command type
    RadioCommandType type = command->getType();

    // check if a method is binded for the given command
    if (this->bindings.find(type) == this->bindings.end())
    {
        // create the exception string
        std::string ex = str(boost::format("no method binding for radio-command-type [ %1% ]") % ((int) type));
        // display the exception with logger
        BOOST_LOG_TRIVIAL(error) << ex;
        // trow an exception
        throw RadioException(ex);
    }

    // execute the binded method wfor the given command
    this->bindings[type](command, radio);
}

// /////////////////////////////////////////////////////////////////////////////

void CommandExecutor::onArmCommand(IRadioCommand *command, AbstractRadio *radio)
{
    // set the arm signal
    radio->setArmSignal();
}

void CommandExecutor::onBindTransmitterCommand(IRadioCommand *command,
                                               AbstractRadio *radio)
{
    // set the bind signal
    radio->setBindSignal();
}

void CommandExecutor::onControlCommand(IRadioCommand *command,
                                       AbstractRadio *radio)
{
    // cast the command to ControlCommand
    if (ControlCommand *crtl = dynamic_cast<ControlCommand *>(command))
    {
        // get the control signals
        cv::Vec4d u = crtl->getSignals();
        // update the controls
        radio->setControls(u[0], u[1], u[2], u[3]);
    }
}

void CommandExecutor::onDisarmCommand(IRadioCommand *command,
                                      AbstractRadio *radio)
{
    // set the disarm signal
    radio->setDisarmSignal();
}

void CommandExecutor::onResumeCopterCommand(IRadioCommand *command,
                                            AbstractRadio *radio)
{
    radio->suspend(false);
}

void CommandExecutor::onSuspendCopterCommand(IRadioCommand *command,
                                             AbstractRadio *radio)
{
    radio->suspend(true);
}

void CommandExecutor::onTakeOverCommand(IRadioCommand *command,
                                        AbstractRadio *radio)
{
}

void CommandExecutor::onToggleSuspensionCommand(IRadioCommand *command,
                                                AbstractRadio *radio)
{
    radio->toggleSuspension();
}

void CommandExecutor::onToggleTransmitterCommand(IRadioCommand *command,
                                                 AbstractRadio *radio)
{
    radio->toggleSender();
}

void CommandExecutor::onEmergencyCommand(IRadioCommand *command,
                                         AbstractRadio *radio)
{
    radio->emergencyStop();
}

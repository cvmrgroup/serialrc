//
// Created by lasse on 29.09.15.
//

#ifndef ICARUS_COMMANDEXECUTOR_H
#define ICARUS_COMMANDEXECUTOR_H

#include <unordered_map>

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/function.hpp>
#include <boost/log/trivial.hpp>

#include <interfaces/i_radiocommand.h>
#include <radio/radiocommands.h>
#include <radio/abstracttxmodule.h>

class CommandExecutor
{

public:

    CommandExecutor();

private:

    void initializeBindings();

    // /////////////////////////////////////////////////////////////////////////

    void onTakeOverCommand(IRadioCommand *command, AbstractTxModule *radio);

    // /////////////////////////////////////////////////////////////////////////

    void onToggleSuspensionCommand(IRadioCommand *command, AbstractTxModule *radio);

    void onSuspendCopterCommand(IRadioCommand *command, AbstractTxModule *radio);

    void onResumeCopterCommand(IRadioCommand *command, AbstractTxModule *radio);

    void onControlCommand(IRadioCommand *command, AbstractTxModule *radio);

    // DSMX ////////////////////////////////////////////////////////////////////

    void onBindTransmitterCommand(IRadioCommand *command, AbstractTxModule *radio);

    void onToggleTransmitterCommand(IRadioCommand *command, AbstractTxModule *radio);

    void onSwitchGearCommand(IRadioCommand *command, AbstractTxModule *radio);

    void onSetGearCommand(IRadioCommand *command, AbstractTxModule *radio);

    void onSwitchAux1Command(IRadioCommand *command, AbstractTxModule *radio);

public:

    /**
     * execute the given IRadioCommand on the given AbstractRadio
     * @param command the command to execute
     * @param radio the radio where the command is executed for
     */
    void execute(IRadioCommand *command, AbstractTxModule *radio);

private:

    std::unordered_map<RadioCommandType, boost::function<void(IRadioCommand *, AbstractTxModule *)>> bindings;
};

#endif //ICARUS_COMMANDEXECUTOR_H

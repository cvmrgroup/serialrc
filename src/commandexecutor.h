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

#include <radio/i_radiocommand.h>
#include <radio/commands/controlcommand.h>

#include "abstractradio.h"
#include "radioexception.h"

class CommandExecutor
{

public:

    CommandExecutor();

private:

    void initializeBindings();

    // /////////////////////////////////////////////////////////////////////////

    void onArmCommand(IRadioCommand *command, AbstractRadio *radio);

    void onBindTransmitterCommand(IRadioCommand *command, AbstractRadio *radio);

    void onControlCommand(IRadioCommand *command, AbstractRadio *radio);

    void onDisarmCommand(IRadioCommand *command, AbstractRadio *radio);

    void onEmergencyCommand(IRadioCommand *command, AbstractRadio *radio);

    void onResumeCopterCommand(IRadioCommand *command, AbstractRadio *radio);

    void onSuspendCopterCommand(IRadioCommand *command, AbstractRadio *radio);

    void onTakeOverCommand(IRadioCommand *command, AbstractRadio *radio);

    void onToggleSuspensionCommand(IRadioCommand *command,
                                   AbstractRadio *radio);

    void onToggleTransmitterCommand(IRadioCommand *command,
                                    AbstractRadio *radio);

public:

    /**
     * execute the given IRadioCommand on the given AbstractRadio
     * @param command the command to execute
     * @param radio the radio where the command is executed for
     */
    void execute(IRadioCommand *command, AbstractRadio *radio);

private:

    std::unordered_map<RadioCommandType, boost::function<void(IRadioCommand *,
                                                              AbstractRadio *)>> bindings;
};

#endif //ICARUS_COMMANDEXECUTOR_H

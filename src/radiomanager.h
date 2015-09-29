//
// Created by lasse on 28.09.15.
//

#ifndef ICARUS_RADIOMANAGER_H
#define ICARUS_RADIOMANAGER_H

#include <vector>
#include <functional>
#include <unordered_map>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <radio/i_radio.h>
#include <radio/commands/controlcommand.h>

#include "arxx.h"
#include "artp.h"
#include "artt.h"
#include "abstractradio.h"
#include "radioexception.h"
#include "crazyradio.h"
#include "crazyradiothread.h"
#include "config/radioconfiguration.h"

class RadioManager : public IRadio
{
public:
    RadioManager();

    ~RadioManager();

private:

    void initializeCommands();

    void createRadio(RadioConfiguration &conf);

    void createTransmitter(const std::string sender);

    // ############################################################################################

    void onArmCommand(IRadioCommand *command);

    void onBindTransmitterCommand(IRadioCommand *command);

    void onControlCommand(IRadioCommand *command);

    void onDisarmCommand(IRadioCommand *command);

    void onResumeCopterCommand(IRadioCommand *command);

    void onSuspendCopterCommand(IRadioCommand *command);

    void onTakeOverCommand(IRadioCommand *command);

    void onToggleSuspensionCommand(IRadioCommand *command);

    void onToggleTransmitterCommand(IRadioCommand *command);

    // ############################################################################################

public:

    void initialize(std::vector<RadioConfiguration> confs);

    void fireRadioCommand(IRadioCommand *command);

    void start();

    void stop();

private:

    CrazyRadioThread *crazyRadioThread;

    std::unordered_map<std::string, ArXX *> transmitters;

    std::unordered_map<int, AbstractRadio *> radios;

    std::unordered_map<RadioCommandType, boost::function<void(IRadioCommand *)>> commands;
};


#endif //ICARUS_RADIOMANAGER_H

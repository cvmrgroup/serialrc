//
// Created by lasse on 28.09.15.
//

#ifndef ICARUS_RADIOMANAGER_H
#define ICARUS_RADIOMANAGER_H

#include <vector>
#include <functional>
#include <unordered_map>

#include <boost/format.hpp>
#include <boost/signals2.hpp>
#include <boost/log/trivial.hpp>

#include <radio/i_radio.h>
#include <radio/commands/controlcommand.h>

#include "transmitter/artp.h"
#include "transmitter/artt.h"
#include "transmitter/crazyradiotransmitter.h"

#include "radio/radioevent.h"

#include "radio/dsmxradio.h"
#include "radio/crazyradio.h"

#include "radioexception.h"
#include "commandexecutor.h"
#include "config/radioconfiguration.h"

class IOServiceRadio : public IRadio
{
public:
    IOServiceRadio(std::vector<RadioConfiguration> configs, boost::shared_ptr<boost::asio::io_service> io_service);

    virtual ~IOServiceRadio();

private:

    void fireRadioEvent(AbstractRadio *radio);

    void createRadio(RadioConfiguration &conf);

    ITransmitter *createAndGetTransmitter(const std::string sender);

    // ############################################################################################

    void doStart();

    void doStop();

    // ############################################################################################

    void executeCommand(IRadioCommand *command);

public:

    void fireRadioCommand(IRadioCommand *command);

    /**
     * starts the IOServiceRadio, which means to open and connect to all configured transmitters
     * the method is thread safe, because the call gets invoke into the underlying io_service.
     */
    void start();

    /**
     * stops the IOServiceRadio, which means to close all connected transmitters,
     * the method is thread safe, because the call gets invoke into the underlying io_service.
     */
    void stop();

    /**
     * waits for the IOServiceRadio to finish
     */
    void join();

private:

    /// the CommandExecutor to execute the correct logic for a received IRadioCommand
    CommandExecutor commandExecutor;

    /// the underlying and used io_service
    boost::shared_ptr<boost::asio::io_service> io_service;

    /// the list with RadioConfiguration
    std::vector<RadioConfiguration> configs;

    /// the map with all radios for the configured copters
    std::unordered_map<int, AbstractRadio *> radios;

    /// the map with all used ITransmitters
    std::unordered_map<std::string, ITransmitter *> transmitters;
};


#endif //ICARUS_RADIOMANAGER_H

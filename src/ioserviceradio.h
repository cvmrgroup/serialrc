//
// Created by lasse on 28.09.15.
//

#ifndef ICARUS_RADIOMANAGER_H
#define ICARUS_RADIOMANAGER_H

#include <vector>
#include <functional>
#include <unordered_map>

#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/signals2.hpp>
#include <boost/log/trivial.hpp>

#include <radio/i_radio.h>
#include <radio/commands/controlcommand.h>

#include "i_transmitter.h"

// dsmx tx based radios (arduino and pi)
#include "abstractradio.h"
#include "dsmxradio.h"

#ifdef WITH_ARDUINO
#include "arduino/artp.h"
#include "arduino/artt.h"
#endif

#ifdef WITH_CRAZYRADIO
#include "crazyradio/crazyradiotransmitter.h"
#include "crazyradio/crazyradio.h"
#endif

#ifdef WITH_RASPBERRYPI
#include "raspberrypi/rpitx.h"
#endif

#include "radio/radioevent.h"

#include "radioexception.h"
#include "commandexecutor.h"
#include "config/radioconfig.h"

class IOServiceRadio : public IRadio
{
public:
    IOServiceRadio(std::vector<RadioConfig> configs,
                   boost::shared_ptr<boost::asio::io_service> io_service);

    virtual ~IOServiceRadio();

private:

    void fireRadioEvent(AbstractRadio *radio);

    void fireTelemetryData(Telemetry data);

    void createRadio(RadioConfig &config);

    ITransmitter *createAndGetTransmitter(const std::string sender);

    // /////////////////////////////////////////////////////////////////////////

    void doStart();

    void doStop();

    // /////////////////////////////////////////////////////////////////////////

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
    std::vector<RadioConfig> configs;

    /// the map with all radios for the configured copters
    std::unordered_map<int, AbstractRadio *> radios;

    /// the map with all used ITransmitters
    std::unordered_map<std::string, ITransmitter *> transmitters;
};

#endif //ICARUS_RADIOMANAGER_H

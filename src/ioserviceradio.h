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

#include <radio/i_transmitter.h>

#include <radio/abstracttxmodule.h>

// dsmx tx based modules (arduino and pi)
#include <arduino/dsmxmodule.h>

// simulation
#include <simulation/dyscotx.h>
#include <simulation/dyscomodule.h>

#ifdef WITH_ARDUINO

#include "arduino/artp.h"
#include "arduino/artt.h"

#endif

#ifdef WITH_CRAZYRADIO

#include "crazyradio/crazyradiotransmitter.h"
#include "crazyradio/crazyradiomodule.h"

#endif

#ifdef WITH_RASPBERRYPI
#include "raspberrypi/rpitx.h"
#endif

#include "radio/radioevent.h"

#include "commandexecutor.h"
#include "config/txmoduleconfig.h"

class IOServiceRadio : public IRadio
{
public:
    IOServiceRadio(std::vector<TxModuleConfig> configs,
                   boost::shared_ptr<boost::asio::io_service> ioService);

    ~IOServiceRadio() override;

private:

    void fireRadioEvent(AbstractTxModule *radio);

    void fireImuData(ImuData data);

    void createRadio(TxModuleConfig &config);

    ITransmitter *createAndGetTransmitter(const TxModuleConfig &config);

    AbstractTxModule *getTxModule(int copterId) override;

    // /////////////////////////////////////////////////////////////////////////

    void doStart();

    void doStop();

    // /////////////////////////////////////////////////////////////////////////

    void executeCommand(IRadioCommand *command);

public:

    void fireRadioCommand(IRadioCommand *command) override;

    /**
     * starts the IOServiceRadio, which means to open and connect to all configured transmitters
     * the method is thread safe, because the call gets invoke into the underlying io_service.
     */
    void start() override;

    /**
     * stops the IOServiceRadio, which means to close all connected transmitters,
     * the method is thread safe, because the call gets invoke into the underlying io_service.
     */
    void stop() override;

    /**
     * waits for the IOServiceRadio to finish
     */
    void join() override;

private:

    /// the executor of received radio commands
    CommandExecutor commandExecutor;

    /// the list of radio configs
    std::vector<TxModuleConfig> configs;

    /// the underlying io_service
    boost::shared_ptr<boost::asio::io_service> ioService;

    /// a map with all used transmitters
    std::unordered_map<std::string, ITransmitter *> transmitters;

    /// a map with all transmitter modules
    std::unordered_map<int, AbstractTxModule *> txModules;
};

#endif //ICARUS_RADIOMANAGER_H

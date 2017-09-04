/*******************************************************************************
 *
 *            #, #,         CCCCCC  VV    VV MM      MM RRRRRRR
 *           %  %(  #%%#   CC    CC VV    VV MMM    MMM RR    RR
 *           %    %## #    CC        V    V  MM M  M MM RR    RR
 *            ,%      %    CC        VV  VV  MM  MM  MM RRRRRR
 *            (%      %,   CC    CC   VVVV   MM      MM RR   RR
 *              #%    %*    CCCCCC     VV    MM      MM RR    RR
 *             .%    %/
 *                (%.      Computer Vision & Mixed Reality Group
 *
 ******************************************************************************/
/**          @copyright:   Hochschule RheinMain,
 *                         University of Applied Sciences
 *              @author:   Marc Lieser
 *             @version:   1.0
 *                @date:   25.08.2017
 ******************************************************************************/

#include <csignal>

#include <vector>
#include <iostream>

#include <boost/regex.hpp>
#include <boost/log/trivial.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/program_options.hpp>

#include <arduino/arxx.h>
#include <arduino/artt.h>
#include <arduino/artp.h>

#ifdef WITH_RASPBERRYPI
#include <raspberrypi/rpitx.h>
#endif

#include "icarus/gamepaddevice.h"

volatile sig_atomic_t signaled = 0;

void signalHandler(int signum)
{
    signaled = 1;
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signalHandler);

    if (argc != 2)
    {
        std::cout << "Usage: ./serialrc-example <serial-port-name>" << std::endl;
        return 0;
    }

    std::string serialPortName(argv[1]);

    GamepadDevice input("Gamepad", 0);
    input.open();

    boost::shared_ptr<boost::asio::io_service> ioService(new boost::asio::io_service());
    boost::asio::io_service::work *work = new boost::asio::io_service::work(*ioService);

    int nChannels = 6;

    // Nano QX Setup ///////////////////////////////////////////////////////////

    TxModuleConfig defaultConfig;
    defaultConfig.txId = 0;
    defaultConfig.serialPort = serialPortName;
    defaultConfig.frameRate = 22;
    defaultConfig.channels = std::vector<ServoSetup>(nChannels);
    defaultConfig.channels[0] = ServoSetup("throttle", 100, false, 0);
    defaultConfig.channels[1] = ServoSetup("roll", 100, false, 0);
    defaultConfig.channels[2] = ServoSetup("pitch", 100, false, 0);
    defaultConfig.channels[3] = ServoSetup("yaw", 100, false, 0);
    defaultConfig.channels[4] = ServoSetup("gear", 100, false, 2);
    defaultConfig.channels[5] = ServoSetup("aux1", 100, false, 2);
    defaultConfig.armSignalProvided = false;
    defaultConfig.disarmSignalProvided = false;

    // Blade Torrent 110 Setup /////////////////////////////////////////////////

    TxModuleConfig torrentConfig;
    torrentConfig.txId = 7;
    torrentConfig.serialPort = serialPortName;
    torrentConfig.frameRate = 11;
    torrentConfig.channels = std::vector<ServoSetup>(nChannels);
    torrentConfig.channels[0] = ServoSetup("throttle", 150, false, 0);
    torrentConfig.channels[1] = ServoSetup("roll", 148, true, 0);
    torrentConfig.channels[2] = ServoSetup("pitch", 148, false, 0);
    torrentConfig.channels[3] = ServoSetup("yaw", 148, true, 0);
    torrentConfig.channels[4] = ServoSetup("gear", 100, false, 3);
    torrentConfig.channels[5] = ServoSetup("aux1", 100, false, 2);

    defaultConfig.armSignalProvided = true;
    defaultConfig.disarmSignalProvided = true;

    torrentConfig.armSignalMap[0] = -1.;
    torrentConfig.armSignalMap[5] = 1.;

    torrentConfig.disarmSignalMap[5] = -1;

    ITransmitter *tx;
    DSMXModule *module;

#ifdef WITH_RASPBERRYPI
    tx = new RpiTX(config.transmitter, config.serialPort, ioService);
    module = new DSMXModule(0, defaultConfig);
#else
    // Example for the dsmx module solution with a default servo configuration
    //tx = new ArTT(defaultConfig.transmitter, defaultConfig.serialPort, ioService);
    //module = new DSMXModule(0, defaultConfig);

    // Example for the trainer port solution with a more sophisticated servo configuration
    tx = new ArTP(defaultConfig.transmitter, torrentConfig.serialPort, ioService);
    module = new DSMXModule(0, torrentConfig);
#endif

    tx->addTxModule(module);
    tx->open();

    // /////////////////////////////////////////////////////////////////////////

    while (!signaled)
    {
        ioService->poll();

        if (!input.update())
        {
            // no update
            continue;
        }

        if (input.checkState(DeviceState::TRANSMITTER))
        {
            ioService->post(boost::bind(&DSMXModule::toggleSender, module));
        }

        if (input.checkState(DeviceState::SUSPENSION))
        {
            ioService->post(boost::bind(&DSMXModule::toggleSuspension, module));
        }

        if (input.checkState(DeviceState::ARM))
        {
            ioService->post(boost::bind(&DSMXModule::setArmSignal, module));
        }

        if (input.checkState(DeviceState::DISARM))
        {
            ioService->post(boost::bind(&DSMXModule::setDisarmSignal, module));
        }

        if (input.checkState(DeviceState::TOGGLE_GEAR))
        {
            ioService->post(boost::bind(&DSMXModule::toggleGear, module));
        }

        if (input.checkState(DeviceState::TOGGLE_AUX1))
        {
            ioService->post(boost::bind(&DSMXModule::toggleAux1, module));
        }

        if (input.checkState(DeviceState::BIND))
        {
            ioService->post(boost::bind(&DSMXModule::setBindSignal, module));
        }
        else
        {
            ControlInput u = input.getControls();
            ioService->post(boost::bind(&DSMXModule::setControls, module, u));
        }
    }

    // /////////////////////////////////////////////////////////////////////////

    input.close();
    tx->close();

    // clean up application
    delete module;
    delete tx;

    BOOST_LOG_TRIVIAL(info) << "Goodbye.";

    // return
    return 0;
}

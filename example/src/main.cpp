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

#include "icarus/gamepaddevice.h"

volatile sig_atomic_t signaled = 0;

void signalHandler(int signum)
{
    signaled = 1;
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signalHandler);

    GamepadDevice input("Gamepad", 0);
    input.open();

    boost::shared_ptr<boost::asio::io_service> ioService(new boost::asio::io_service());
    boost::asio::io_service::work *work = new boost::asio::io_service::work(*ioService);

    TxModuleConfig config;

    config.id = 0;
    config.txId = 0;
    config.copterId = 0;
    config.transmitter = "artt";
    config.serialPort = "/dev/cu.usbmodem1421";

    config.frameRate = 22;

    config.channels = std::vector<ServoSetup>(6);
    config.channels[0] = ServoSetup("throttle", 100, false, 0);
    config.channels[1] = ServoSetup("roll", 100, false, 0);
    config.channels[2] = ServoSetup("pitch", 100, false, 0);
    config.channels[3] = ServoSetup("yaw", 100, false, 0);
    config.channels[4] = ServoSetup("gear", 100, false, 2);
    config.channels[5] = ServoSetup("aux1", 100, false, 2);

    config.armSignalProvided = false;
    config.disarmSignalProvided = false;

    ArXX *transmitter = new ArTT(config.transmitter, config.serialPort, ioService);

    DSMXModule *module = new DSMXModule(0, config);
    transmitter->addTxModule(module);

    transmitter->open();

    // /////////////////////////////////////////////////////////////////////////

    while (!signaled)
    {
        ioService->poll();

        if (!input.update())
        {
            // no update
            continue;
        }

        if (input.checkState(DeviceState::SUSPENSION))
        {
            ioService->post(boost::bind(&DSMXModule::toggleSuspension, module));
        }

        if (input.checkState(DeviceState::TRANSMITTER))
        {
            ioService->post(boost::bind(&DSMXModule::toggleSender, module));
        }

        if (input.checkState(DeviceState::BIND))
        {
            ioService->post(boost::bind(&DSMXModule::setBindSignal, module));
        }
        else if (input.checkState(DeviceState::TOGGLE_GEAR))
        {
            ioService->post(boost::bind(&DSMXModule::toggleGear, module));
        }
        else if (input.checkState(DeviceState::TOGGLE_AUX1))
        {
            ioService->post(boost::bind(&DSMXModule::toggleAux1, module));
        }
        else
        {
            ControlInput u = input.getControls();
            ioService->post(boost::bind(&DSMXModule::setControls, module, u));
        }
    }

    // /////////////////////////////////////////////////////////////////////////

    input.close();
    transmitter->close();

    // clean up application
    delete module;
    delete transmitter;

    BOOST_LOG_TRIVIAL(info) << "Goodbye.";

    // return
    return 0;
}

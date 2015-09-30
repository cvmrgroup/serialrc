//
// Created by lasse on 28.09.15.
//

#ifndef ICARUS_INPUTMANAGER_H
#define ICARUS_INPUTMANAGER_H

#include <vector>
#include <exception>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <gamepad.h>

#include <radio/i_radio.h>
#include <radio/commands/armcommand.h>
#include <radio/commands/disarmcommand.h>
#include <radio/commands/controlcommand.h>
#include <radio/commands/suspendcoptercommand.h>
#include <radio/commands/bindtransmittercommand.h>
#include <radio/commands/togglesuspensioncommand.h>
#include <radio/commands/toggletransmittercommand.h>

#include <config/radioconfiguration.h>

#include <radioexception.h>

class InputManager
{
public:

    InputManager(int source, std::vector<RadioConfiguration> confs, IRadio *radio, boost::shared_ptr<boost::asio::io_service> io_service);

private:

    void update(const boost::system::error_code &ec);

    void updateGamepad();

    void invokeTimer();

public:

    void start();

    void stop();

private:

    int source;
    int confId;

    IRadio *radio;
    Gamepad gamepad;

    std::vector<RadioConfiguration> confs;

    boost::shared_ptr<boost::asio::io_service> io_service;
    boost::shared_ptr<boost::asio::deadline_timer> timer;

};


#endif //ICARUS_INPUTMANAGER_H

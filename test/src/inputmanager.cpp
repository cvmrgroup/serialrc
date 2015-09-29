//
// Created by lasse on 28.09.15.
//

#include "inputmanager.h"

InputManager::InputManager(int source, int copterId, IRadio *radio,
                           boost::shared_ptr<boost::asio::io_service> io_service)
{
    this->radio = radio;
    this->source = source;
    this->copterId = copterId;
    this->io_service = io_service;
    this->timer = boost::shared_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(*io_service));
}

void InputManager::start()
{
    // open the gamepad
    if (!this->gamepad.open())
    {
        BOOST_LOG_TRIVIAL(error) << "fail to open gamepad";
        return;
    }
    // invoke the timer
    this->invokeTimer();
}

void InputManager::stop()
{
    // check if the timer exists
    if (this->timer)
    {
        // stop the timer
        this->timer->cancel();
    }
    // close the gamepad
    this->gamepad.close();
}

// ########################################################################################

void InputManager::invokeTimer()
{
    // get a delay from 10 ms
    boost::posix_time::milliseconds timeout(10);
    this->timer->expires_from_now(timeout);
    // add the async callback for the timer
    this->timer->async_wait(boost::bind(&InputManager::update, this, boost::asio::placeholders::error));

}

void InputManager::update(const boost::system::error_code &ec)
{
    // check if an error happens
    if (ec)
    {
        std::string ex = str(boost::format("fail to update input manager, because [ %1% ]") % ec.message());
        // display error
        BOOST_LOG_TRIVIAL(error) << ex;
        // throw the exception
        throw RadioException(ex);
    }

    if (this->gamepad.update())
    {
        this->updateGamepad();
    }

    this->invokeTimer();
}

void InputManager::updateGamepad()
{
    if (this->gamepad.buttonToggled(BUTTON_START))
    {
        // toggle play/pause
        radio->fireRadioCommand(new ToggleSuspensionCommand(this->source, this->copterId));
    }

    if (this->gamepad.buttonToggled(BUTTON_SQUARE))
    {
        // toggle tansmitter on/off
        radio->fireRadioCommand(new ToggleTransmitterCommand(this->source, this->copterId));
    }

    if (this->gamepad.buttonToggled(BUTTON_SELECT))
    {
        this->io_service->stop();
        BOOST_LOG_TRIVIAL(info) << "stop";
    }

    IRadioCommand *command = NULL;

    if (this->gamepad.buttonPressed(BUTTON_RIGHT))
    {
        // set bind header
        command = new BindTransmitterCommand(this->source, this->copterId);
    }
    else if (this->gamepad.buttonPressed(BUTTON_UP))
    {
        // arm multiwii
        command = new ArmCommand(this->source, this->copterId);
    }
    else if (this->gamepad.buttonPressed(BUTTON_DOWN))
    {
        command = new DisarmCommand(this->source, this->copterId);
    }
    else
    {
        // set control signals
        cv::Vec4d u = this->gamepad.getAxisControls();
        command = new ControlCommand(this->source, this->copterId, u);
    }

    // check if the command is not null
    if (command != NULL)
    {
        // fire the command
        radio->fireRadioCommand(command);
    }
}


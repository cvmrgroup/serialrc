//
// Created by lasse on 28.09.15.
//

#include "inputmanager.h"

InputManager::InputManager(int source, std::vector<RadioConfiguration> confs,
                           IRadio *radio,
                           boost::shared_ptr<boost::asio::io_service> io_service)
{
    this->confId = 0;
    this->radio = radio;
    this->confs = confs;
    this->source = source;
    this->io_service = io_service;
    this->timer = boost::shared_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(*io_service));
}

void InputManager::start()
{
    // open the gamepad
    if (!this->gamepad.open())
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to open gamepad.";
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

// /////////////////////////////////////////////////////////////////////////////

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
        std::string ex = str(boost::format("Updating input manager failed with error code [ %1% ].") % ec.message());
        // display error
        BOOST_LOG_TRIVIAL(error) << ex;
        // throw the exception
        throw RadioException(ex);
    }

    if (this->gamepad.update() && !this->confs.empty())
    {
        this->updateGamepad();
    }

    this->invokeTimer();
}

void InputManager::updateGamepad()
{
    RadioConfiguration conf = this->confs[this->confId];

    if (this->gamepad.buttonToggled(BUTTON_START))
    {
        // toggle play/pause
        radio->fireRadioCommand(new ToggleSuspensionCommand(this->source, conf.copterId));
    }

    if (this->gamepad.buttonToggled(BUTTON_SQUARE))
    {
        // toggle tansmitter on/off
        radio->fireRadioCommand(new ToggleTransmitterCommand(this->source, conf.copterId));
    }

    if (this->gamepad.buttonToggled(BUTTON_TRIANGLE) && this->confs.size() > 1)
    {
        radio->fireRadioCommand(new SuspendCopterCommand(this->source, conf.copterId));
        this->confId = (this->confId + 1) % this->confs.size();
        conf = this->confs[this->confId];

        BOOST_LOG_TRIVIAL(info) << "switch copter to [ id: " << conf.copterId << ", sender: " << conf.sender << ", txId: " << conf.txId << " ]";
    }

    if (this->gamepad.buttonToggled(BUTTON_SELECT))
    {
        this->io_service->stop();
    }

    IRadioCommand *command = NULL;

    if (this->gamepad.buttonPressed(BUTTON_RIGHT))
    {
        // set bind header
        command = new BindTransmitterCommand(this->source, conf.copterId);
    }
    else if (this->gamepad.buttonPressed(BUTTON_UP))
    {
        // arm multiwii
        command = new ArmCommand(this->source, conf.copterId);
    }
    else if (this->gamepad.buttonPressed(BUTTON_DOWN))
    {
        command = new DisarmCommand(this->source, conf.copterId);
    }
    else
    {
        // set control signals
        cv::Vec4d u = this->gamepad.getAxisControls();
        command = new ControlCommand(this->source, conf.copterId, u);
    }

    // check if the command is not null
    if (command != NULL)
    {
        // fire the command
        radio->fireRadioCommand(command);
    }
}


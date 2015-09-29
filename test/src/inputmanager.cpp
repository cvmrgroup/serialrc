//
// Created by lasse on 28.09.15.
//

#include "inputmanager.h"

InputManager::InputManager(int source, int copterId, IRadio *radio) : QObject(NULL)
{
    this->source = source;
    this->copterId = copterId;
    this->radio = radio;
}

void InputManager::start()
{
    this->gamepad.open();

    QObject::connect(&this->timer, SIGNAL(timeout()), this, SLOT(update()));
    this->timer.start(10);

}

#include <iostream>

void InputManager::update()
{
    if (!this->gamepad.update())
    {
        return;
    }

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
        QCoreApplication::exit(0);
    }

    if (this->gamepad.buttonToggled(BUTTON_TRIANGLE))
    {
        std::cout << "kk" << std::endl;
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

void InputManager::stop()
{
    this->timer.stop();
    this->gamepad.close();
}
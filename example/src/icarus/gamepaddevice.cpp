//
// Created by lasse on 01.10.15.
//

#include "gamepaddevice.h"

GamepadDevice::GamepadDevice(const std::string &name, int device)
        : InputDevice(name),
          device(device),
          gamepad(nullptr)
{
    this->upperThrust = true; // todo make configurable

    this->rc[ControlInput::Input::THRUST] = RC(1., 1.);
    this->rc[ControlInput::Input::ROLL] = RC(1., 1.);
    this->rc[ControlInput::Input::PITCH] = RC(1., 1.);
    this->rc[ControlInput::Input::YAW] = RC(1., 1.);
}

GamepadDevice::~GamepadDevice()
{
    delete this->gamepad;
}

void GamepadDevice::open()
{
    if (this->gamepad == nullptr)
    {
        this->gamepad = new Gamepad();
    }

    this->opened = this->gamepad->open(this->device);

    if (!this->opened)
    {
        std::string msg = "Failed to open gamepad [ device: " + std::to_string(this->device) + ", name: " + this->getGivenName() + " ].";
        throw std::runtime_error(msg);
    }

    // set mapping
    std::string deviceName = this->gamepad->getDeviceName();

    // DualShock 3
    if (deviceName == "PLAYSTATION(R)3 Controller"
        || deviceName == "Sony PLAYSTATION(R)3 Controller") // linux
    {
        this->buttonMap[DeviceState::ARM] = 12; // TRIANGLE
        this->buttonMap[DeviceState::DISARM] = 14; // CROSS
        this->buttonMap[DeviceState::TRANSMITTER] = 15; // SQUARE
        this->buttonMap[DeviceState::BIND] = 13; // CIRCLE
        this->buttonMap[DeviceState::SUSPENSION] = 3; // START
        this->buttonMap[DeviceState::TOGGLE_GEAR] = 10; // L1
        this->buttonMap[DeviceState::TOGGLE_AUX1] = 11; // R1
    }
    else
    {
        // DualShock 4
        this->buttonMap[DeviceState::ARM] = 3; // TRIANGLE
        this->buttonMap[DeviceState::DISARM] = 1; // CROSS
        this->buttonMap[DeviceState::TRANSMITTER] = 0; // SQUARE
        this->buttonMap[DeviceState::BIND] = 2; // CIRCLE
        this->buttonMap[DeviceState::SUSPENSION] = 9; // OPTIONS
        this->buttonMap[DeviceState::TOGGLE_GEAR] = 5; // L1
        this->buttonMap[DeviceState::TOGGLE_AUX1] = 4; // R1
    }

    this->axesMap[ControlInput::THRUST] = 1; // AXIS_LEFT_Y
    this->axesMap[ControlInput::ROLL] = 2; // AXIS_RIGHT_X
    this->axesMap[ControlInput::PITCH] = 3; // AXIS_RIGHT_Y
    this->axesMap[ControlInput::YAW] = 0; // AXIS_LEFT_X
}

bool GamepadDevice::isOpen()
{
    return this->gamepad != nullptr && this->opened;
}

bool GamepadDevice::update()
{
    if (this->isOpen())
    {
        return this->gamepad->update();
    }

    return false;
}

void GamepadDevice::validGamepadOpen()
{
    if (!this->isOpen())
    {
        std::string msg = "Gamepad [ device: " + std::to_string(this->device) + ", name: " + this->getGivenName() + " ] is not initialized.";
        throw std::runtime_error(msg);
    }
}

bool GamepadDevice::checkState(DeviceState state)
{
    // check if the gamepad is initialized
    this->validGamepadOpen();

    // debug button indices
    // this->gamepad->printDeviceState();

    if (this->buttonMap.find(state) != this->buttonMap.end())
    {
        return this->gamepad->buttonPressed(this->buttonMap[state]);
    }

    if (state == DeviceState::NEUTRAL_POSITION)
    {
        return this->gamepad->inNeutralPosition();
    }

    throw std::runtime_error("Unknown device state given.");
}

ControlInput GamepadDevice::getControls()
{
    this->validGamepadOpen();

    std::vector<float> controls = this->gamepad->getAxisControls();

    ControlInput u;
    u.thrust = controls[this->axesMap[ControlInput::Input::THRUST]];
    u.roll = controls[this->axesMap[ControlInput::Input::ROLL]];
    u.pitch = controls[this->axesMap[ControlInput::Input::PITCH]];
    u.yaw = controls[this->axesMap[ControlInput::Input::YAW]];

    // change thrust range before applying expo
    if (this->upperThrust)
    {
        // just discard the negative lever positions
        u.thrust = u.thrust < 0. ? 0. : u.thrust;
    }
    else
    {
        // map to range 0..1
        u.thrust = (u.thrust + 1.) / 2.;
    }

    // apply expo and dual rate
    u.thrust = this->rc[ControlInput::Input::THRUST].getControl(u.thrust);
    // re-map to range -1..1
    u.thrust = u.thrust * 2. - 1.;

    // apply rate expos and dual rates
    u.roll = this->rc[ControlInput::Input::THRUST].getControl(u.roll);
    u.pitch = this->rc[ControlInput::Input::PITCH].getControl(u.pitch);
    u.yaw = this->rc[ControlInput::Input::YAW].getControl(u.yaw);

    return u;
}

void GamepadDevice::close()
{
    this->opened = false;
    this->gamepad->close();
}

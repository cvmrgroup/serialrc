//
// Created by lasse on 01.10.15.
//

#ifndef ICARUS_GAMEPADDEVICE_H
#define ICARUS_GAMEPADDEVICE_H

#include <map>
#include <atomic>

#include "icarus/controlinput.h"

#include "rc.h"
#include "gamepad.h"
#include "inputdevice.h"

class GamepadDevice : public InputDevice
{

public:

    GamepadDevice(const std::string &name, int device);

    ~GamepadDevice() override;

private:

    void validGamepadOpen();

public:

    void open() override;

    bool isOpen() override;

    bool update() override;

    bool checkState(DeviceState state) override;

    ControlInput getControls() override;

    void close() override;

private:

    int device;

    std::atomic<bool> opened;

    Gamepad *gamepad;

    bool upperThrust;

    std::map<DeviceState, int> buttonMap;
    std::map<ControlInput::Input, int> axesMap;
    std::map<ControlInput::Input, RC> rc;
};

#endif //ICARUS_GAMEPADDEVICE_H

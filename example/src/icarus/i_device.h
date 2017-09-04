//
// Created by lasse on 01.10.15.
//

#ifndef ICARUS_IDEVICE_H
#define ICARUS_IDEVICE_H

#include <string>

struct ControlInput;

enum class DeviceState
{
    SUSPENSION = 0,
    TRANSMITTER,
    BIND,
    ARM,
    DISARM,
    NEUTRAL_POSITION,
    TOGGLE_GEAR,
    TOGGLE_AUX1
};

class IDevice
{

public:

    virtual ~IDevice() = default;

    /**
     * gets the name of the device
     * @return the name of the device
     */
    virtual std::string getGivenName() = 0;

    /**
     * updates the IDevice, which returns true if successful updated
     * @return true if successful updated, otherwise false
     */
    virtual bool update() = 0;

    /**
     * checks if the given DeviceState is active
     * @param state the DeviceState to request the state
     * @return true if the given device state is active, otherwise false
     */
    virtual bool checkState(DeviceState state) = 0;

    /**
     * gets the current Controls of the IDevice
     * @return the current controls (throttle, roll, pitch, yaw)
     */
    virtual ControlInput getControls() = 0;
};

#endif //ICARUS_IDEVICE_H

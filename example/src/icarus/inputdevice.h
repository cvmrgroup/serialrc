//
// Created by lasse on 01.10.15.
//

#ifndef ICARUS_INPUTDEVICE_H
#define ICARUS_INPUTDEVICE_H

#include "i_device.h"

class InputDevice : public IDevice
{

public:

    /**
     * Constructor.
     * @param name The device's name.
     */
    explicit InputDevice(const std::string &name);

    /**
     * Get the device's name.
     * @return The device's name.
     */
    std::string getGivenName() override;

    /**
     * Opens the device.
     */
    virtual void open() = 0;

    /**
     * Close the device.
     */
    virtual void close() = 0;

    /**
     * Checks if the device is opened.
     * @return True if the device is opened, otherwise false.
     */
    virtual bool isOpen() = 0;

private:

    /// the device's given name
    std::string givenName;
};

#endif //ICARUS_INPUTDEVICE_H

//
// Created by lasse on 01.10.15.
//

#include "inputdevice.h"

InputDevice::InputDevice(const std::string &name)
{
    this->givenName = name;
}

std::string InputDevice::getGivenName()
{
    return this->givenName;
}
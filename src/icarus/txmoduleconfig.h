//
// Created by lasse on 28.09.15.
//

#ifndef ICARUS_RADIOCONFIG_H
#define ICARUS_RADIOCONFIG_H

#include <string>
#include <vector>

#include "servosetup.h"

struct TxModuleConfig
{
    TxModuleConfig()
            : initialized(false)
    {
    }

    /// Flag if the transmitter was initialized
    bool initialized;

    /// The config id
    int id;

    /// The transmitter id
    int txId;

    /// crazyflie radio link
    std::string radioLink;

    /// The copter id
    int copterId;

    /// The name of the transmitter type
    std::string transmitter;

    /// The serial number of the device to distinguish between trainer port and dsmx module approaches.
    std::string serialNumber;

    /// The serial port name the Arduino is connected to
    std::string serialPort;

    /// DSMX frame rate
    int frameRate;

    /// The channel settings
    std::vector<ServoSetup> channels;

    /**
     * In case of brushless quadrotors, an arm signal has to be provided. Small
     * quadrotors only use a makeshift disarm signal with suppressed throttle.
     */
    bool armSignalProvided;

    /// Array with arm signal relevant flags.
    std::vector<bool> armSignalMask;

    /// The complete arm signal
    std::vector<double> armSignal;

    /// Did the config provide a disarm signal too?
    bool disarmSignalProvided;

    /// Array with arm signal relevant flags
    std::vector<bool> disarmSignalMask;

    /// The complete disarm signal
    std::vector<double> disarmSignal;
};

#endif //ICARUS_RADIOCONFIG_H

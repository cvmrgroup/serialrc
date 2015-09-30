//
// Created by lasse on 30.09.15.
//

#ifndef ICARUS_RADIOEVENT_H
#define ICARUS_RADIOEVENT_H

#include <string>

class RadioEvent
{

public:

    RadioEvent()
    { };

    /// the copter id where the radio is binded to
    int copterId;

    /// the transmitter id
    int txId;

    /// if the radio is enabled
    bool enabled;

    /// if the radio is at binding
    bool binding;

    /// if the radio is suspended
    bool suspended;

    double ch5;
    double ch6;

};

#endif //ICARUS_RADIOEVENT_H

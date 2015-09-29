//
// Created by lasse on 28.09.15.
//

#ifndef ICARUS_RADIOCONFIGURATION_H
#define ICARUS_RADIOCONFIGURATION_H

#include <string>

class RadioConfiguration
{

public:

    RadioConfiguration()
    { };

    /// the transmitter id
    int txId;

    /// the copter id
    int copterId;

    /// the name of the transmitter type
    std::string sender;
};

#endif //ICARUS_RADIOCONFIGURATION_H

//
// Created by lasse on 28.09.15.
//

#ifndef ICARUS_RADIOCONFIGLOADER_H
#define ICARUS_RADIOCONFIGLOADER_H

#include <vector>

#include <opencv2/core.hpp>

#include "radioconfiguration.h"

class RadioConfigLoader
{
private:

    RadioConfigLoader();

public:

    static std::vector<RadioConfiguration> loadConfig(const std::string &file);
};


#endif //ICARUS_RADIOCONFIGLOADER_H

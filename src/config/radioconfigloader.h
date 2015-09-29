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

    /**
     * loads all RadioConfigurations from the given file
     * @param the file to the configuration
     * @return a vector with all loaded RadioConfigurations
     */
    static std::vector<RadioConfiguration> loadConfig(const std::string &file);
};


#endif //ICARUS_RADIOCONFIGLOADER_H

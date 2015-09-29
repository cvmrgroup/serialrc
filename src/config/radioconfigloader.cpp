//
// Created by lasse on 28.09.15.
//

#include "radioconfigloader.h"

RadioConfigLoader::RadioConfigLoader()
{ }


std::vector<RadioConfiguration> RadioConfigLoader::loadConfig(const std::string &file)
{
    std::vector<RadioConfiguration> confs;

    cv::FileStorage fsConfiguration(file, cv::FileStorage::READ);

    int nCopter = (int) fsConfiguration["n_copter"];

    for (int i = 0; i < nCopter; i++)
    {
        std::stringstream ss;
        ss << "copter_" << i;

        std::string sender;
        int txId;

        cv::FileNode n = fsConfiguration[ss.str()];

        n["sender"] >> sender;
        txId = (int) n["tx_id"];

        RadioConfiguration configuration;

        configuration.txId = txId;
        configuration.copterId = i;
        configuration.sender = sender;

        confs.push_back(configuration);
    }

    fsConfiguration.release();

    return confs;
}
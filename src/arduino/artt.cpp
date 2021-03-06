/******************************************************
 *
 *   #, #,         CCCCCC  VV    VV MM      MM RRRRRRR
 *  %  %(  #%%#   CC    CC VV    VV MMM    MMM RR    RR
 *  %    %## #    CC        V    V  MM M  M MM RR    RR
 *   ,%      %    CC        VV  VV  MM  MM  MM RRRRRR
 *   (%      %,   CC    CC   VVVV   MM      MM RR   RR
 *     #%    %*    CCCCCC     VV    MM      MM RR    RR
 *    .%    %/
 *       (%.      Computer Vision & Mixed Reality Group
 *
 *****************************************************/
/** @copyright:   Hochschule RheinMain,
 *                University of Applied Sciences
 *     @author:   Marc Lieser
 *    @version:   1.0
 *       @date:   13.05.2014
 *****************************************************/

#include "artt.h"

ArTT::ArTT(std::string name,
           std::string serial,
           boost::shared_ptr<boost::asio::io_service> ioService)
        : ArXX(name, ATT_MAX_RADIOS, serial, ioService)
{
    this->first = true;
    this->lastModuleId = -1;
    this->token = 0;
}

void ArTT::addTxModule(AbstractTxModule *module)
{
    int id = module->getModuleId();
    DSMXModule *dsmx = static_cast<DSMXModule *>(module);

    this->configs[id] = dsmx->getConfig();
    this->moduleIds.push_back(id);

    if (this->hasCapacity())
    {
        this->modules[id] = dsmx;
    }
    else
    {
        std::string msg = boost::str(boost::format("Cannot add txModule with transmitter id [ %1% ] to serial port [ %2% ]. No capacity free.") % id % this->portName);
        throw std::runtime_error(msg);
    }
}

void ArTT::finalize()
{
    // todo switch off transmitters
}

void ArTT::onData(std::string frame)
{
    // get current transmitter id, increment token, remember tx for next call
    int moduleId = this->moduleIds[this->token];
    this->token = (this->token + 1) % (unsigned int) this->configs.size();

    // just ignore the first message (kind of flush serial buffer)
    if (this->first)
    {
        BOOST_LOG_TRIVIAL(info) << "First serial message ignored successfully [ " << frame << " ].";
        this->first = false;
    }
    else if (frame[2] == AXX_DELIMITER)
    {
        int responseId = (int) frame[0];
        int responseType = (int) frame[1];

        if (this->lastModuleId != responseId)
        {
            BOOST_LOG_TRIVIAL(error) << "Reply did not match last command [ " << this->lastModuleId << "!=" << moduleId << " ].";
        }

        if (this->lastModuleId == responseId && responseType == ATT_FRAME_TYPE_CONFIG)
        {
            this->configs[this->lastModuleId].initialized = true;
            BOOST_LOG_TRIVIAL(info) << "Transmitter [ " << this->lastModuleId << " ] configured.";
        }
    }
    else
    {
        std::string msg = boost::str(boost::format("Received frame is not of expected type, name [ %1% ], message [ %2% ].") % this->getName() % frame);
        throw std::runtime_error(msg);
    }

    this->writeData(moduleId);

    this->lastModuleId = moduleId;
}

void ArTT::writeData(int id)
{
    // check if a radio exists for the given id
    if (this->modules.find(id) == this->modules.end())
    {
        std::string msg = boost::str(boost::format("Failed to write data to transmitter with id [ %1% ] on serial port [ %2% ]: No radio set for transmitter.") % id % this->portName);
        throw std::runtime_error(msg);
    }

    AbstractTxModule *radio = this->modules[id];
    TxModuleConfig &config = this->configs[id];

    unsigned char frame[ATT_FRAME_LENGTH] = { 0 };
    frame[ATT_TX_ID] = (unsigned char) id;

    if (!config.initialized)
    {
        frame[ATT_FRAME_TYPE] = (unsigned char) ATT_FRAME_TYPE_CONFIG;
        frame[ATT_FRAME_RATE] = (unsigned char) config.frameRate;
    }
    else
    {
        frame[ATT_FRAME_TYPE] = (unsigned char) ATT_FRAME_TYPE_SIGNAL;
        frame[ATT_RELAY_STATE] = radio->isEnabled() ? (unsigned char) ATT_RELAY_STATE_ON : (unsigned char) ATT_RELAY_STATE_OFF;

        frame[ATT_HEADER_1] = (unsigned char) (radio->isBinding() ? header_1_bind_mode : header_1_default);
        frame[ATT_HEADER_2] = (unsigned char) header_2_default;

        std::vector<double> signal = radio->getSignal();

        int ch1 = ch1_offset + center_value_offset + int(signal[AbstractTxModule::ChannelNames::Throttle] * value_range_scale);
        int ch2 = ch2_offset + center_value_offset + int(signal[AbstractTxModule::ChannelNames::Aileron] * value_range_scale);
        int ch3 = ch3_offset + center_value_offset + int(signal[AbstractTxModule::ChannelNames::Elevation] * value_range_scale);
        int ch4 = ch4_offset + center_value_offset + int(signal[AbstractTxModule::ChannelNames::Rudder] * value_range_scale);
        int ch5 = ch5_offset + center_value_offset + int(signal[AbstractTxModule::ChannelNames::Gear] * value_range_scale);
        int ch6 = ch6_offset + center_value_offset + int(signal[AbstractTxModule::ChannelNames::Aux1] * value_range_scale);

        // write to byte frame
        frame[ATT_CH1_HI] = SerialHelper::hiByte(ch1);
        frame[ATT_CH1_LO] = SerialHelper::loByte(ch1);
        frame[ATT_CH2_HI] = SerialHelper::hiByte(ch2);
        frame[ATT_CH2_LO] = SerialHelper::loByte(ch2);
        frame[ATT_CH3_HI] = SerialHelper::hiByte(ch3);
        frame[ATT_CH3_LO] = SerialHelper::loByte(ch3);
        frame[ATT_CH4_HI] = SerialHelper::hiByte(ch4);
        frame[ATT_CH4_LO] = SerialHelper::loByte(ch4);
        frame[ATT_CH5_HI] = SerialHelper::hiByte(ch5);
        frame[ATT_CH5_LO] = SerialHelper::loByte(ch5);
        frame[ATT_CH6_HI] = SerialHelper::hiByte(ch6);
        frame[ATT_CH6_LO] = SerialHelper::loByte(ch6);
    }

    // write to serial port
    this->writeFrame(reinterpret_cast<const char *>(frame), (size_t) ATT_FRAME_LENGTH);
}

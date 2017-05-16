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

ArTT::ArTT(std::string name, std::string serial,
           boost::shared_ptr<boost::asio::io_service> io_service) :
        ArXX(name, ATT_MAX_RADIOS, serial, io_service)
{
    this->first = true;
    this->lastId = -1;
    this->token = 0;
}

void ArTT::addRadio(AbstractRadio *radio)
{
    // this is a dsmx radio
    DSMXRadio *dsmx = static_cast<DSMXRadio *>(radio);

    // add the radio config to our map of configs
    this->configs.push_back(dsmx->getRadioConfig());

    // notify ArXX about add Radio
    ArXX::addRadio(radio);
}

void ArTT::close()
{
    // todo turn off relays somehow

    ArXX::close();
}

void ArTT::onData(const char *frame, size_t length)
{
    // get current transmitter id, increment token, remember tx for next call
    int tx = this->configs[this->token].id;
    this->token = (this->token + 1) % (unsigned int) this->configs.size();
    this->lastId = tx;

    // just ignore the first message (kind of flush serial buffer)
    if (this->first)
    {
        BOOST_LOG_TRIVIAL(info) << "First serial message ignored successfully [ " << std::string(frame) << " ].";

        this->first = false;
    }
    else if (length == 3 && frame[2] == ATT_DELIMITER)
    {
        int id = (int) frame[0];
        int type = (int) frame[1];

        if (this->lastId == id && type == ATT_FRAME_TYPE_CONFIG)
        {
            this->configs[id].initialized = true;
            BOOST_LOG_TRIVIAL(info) << "Transmitter [ " << id << " ] configured.";
        }
    }
    else
    {
        std::string ex = boost::str(boost::format("Received frame is not of expected type, S/N [ %1% ], message [ %2% ].") % this->getSerialNumber() % std::string(frame));
        BOOST_LOG_TRIVIAL(info) << ex;
        throw RadioException(ex);
    }

    this->writeData(tx);
}

void ArTT::writeData(int id)
{
    // check if an radio for the given id exists
    if (this->radios.find(id) == this->radios.end())
    {
        std::string ex = boost::str(boost::format("Failed to write data to transmitter with id [ %1% ] on serial port [ %2% ]: No radio set for transmitter.") % id % this->getSerialNumber());
        BOOST_LOG_TRIVIAL(error) << ex;
        throw RadioException(ex);
    }

    AbstractRadio *radio = this->radios[id];
    RadioConfig &config = this->configs[id];

    unsigned char frame[ATT_FRAME_LENGTH] = {0};
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
        frame[ATT_HEADER_2] = header_2_default;

        int ch1 = ch1_offset + center_value_offset + int(radio->getThrottle() * value_range_scale);
        int ch2 = ch2_offset + center_value_offset + int(radio->getRoll() * value_range_scale);
        int ch3 = ch3_offset + center_value_offset + int(radio->getPitch() * value_range_scale);
        int ch4 = ch4_offset + center_value_offset + int(radio->getYaw() * value_range_scale);
        int ch5 = ch5_offset + center_value_offset + int(radio->getGear() * value_range_scale);
        int ch6 = ch6_offset + center_value_offset + int(radio->getAux1() * value_range_scale);

        // overwrite throttle signal if copter is supended
        if (radio->isSuspended())
        {
            ch1 = ch1_offset + center_value_offset + int(-1.0 * value_range_scale);
        }

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

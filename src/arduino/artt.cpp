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

ArTT::ArTT(const std::string name, std::string serial,
           boost::shared_ptr<boost::asio::io_service> io_service) :
        ArXX(name, maxRadios, serial, io_service)
{
    this->token = 0;
}

void ArTT::addRadio(AbstractRadio *radio)
{
    // get the transmitter id as string
    std::string txIdStr = radio->getTxId();
    // cast the id to int
    int txId = boost::lexical_cast<int>(txIdStr);
    // add the transmitter id to the ids
    this->transmitterIds.push_back(txId);
    // notify ArXX about add Radio
    ArXX::addRadio(radio);
}

void ArTT::onData(const char *frame, size_t length)
{
    if (length <= 0)
    {
        return;
    }

    // the index to find the xon in the frame
    int xOnIndex = 0;

    /*
     * #########
     * fix for the problem that when the arduino is first time connected,
     * that a frame of size 2 is received with the array:
     * [ -1, 17 ]
     * #########
     */
    if (length > 1)
    {
        xOnIndex = 1;
    }

    // check if XON was received
    if (frame[xOnIndex] == XON)
    {
        // switch to nect token
        this->token = (this->token + 1) % (unsigned int) this->transmitterIds.size();
        // get the next transmitter id
        unsigned int txId = this->transmitterIds[this->token];
        // write for the next transmitter
        this->writeData(txId);
    }
    else
    {
        // create the exception string
        std::string ex = str(boost::format("no XON on serial port [ %1% ] received") % this->getSerialNumber());
        // throw the radio exception
        throw RadioException(ex);
    }
}

void ArTT::writeData(unsigned int id)
{
    // check if an radio for the given id exists
    if (this->radios.find(id) == this->radios.end())
    {
        // create the exception string
        std::string ex = str(
                boost::format("fail to write data to transmitter with id [ %1% ] at serial port [ %2% ], because no radio registered for given transmitter id") %
                id % this->getSerialNumber());
        // display the error
        BOOST_LOG_TRIVIAL(error) << ex;
        // throw an radio exception
        throw RadioException(ex);
    }

    AbstractRadio *radio = this->radios[id];

    unsigned char frame[command_length];

    frame[header_1] = (unsigned char) (radio->isBinding() ? header_1_bind_mode : header_1_default);
    frame[header_2] = header_2_default;
    frame[relay_state] = radio->isEnabled() ? relay_state_on : relay_state_off;
    frame[tx_id] = (unsigned char) id;

    int ch1 = ch1_offset + center_value_offset + int(radio->getThrottle() * value_range_scale);
    int ch2 = ch2_offset + center_value_offset + int(radio->getRoll() * value_range_scale);
    int ch3 = ch3_offset + center_value_offset + int(radio->getPitch() * value_range_scale);
    int ch4 = ch4_offset + center_value_offset + int(radio->getYaw() * value_range_scale);
    int ch5 = ch5_offset + center_value_offset + int(radio->getCh5() * value_range_scale);
    int ch6 = ch6_offset + center_value_offset + int(radio->getCh6() * value_range_scale);

    // overwrite throttle signal if copter is supended
    if (radio->isSuspended())
    {
        ch1 = ch1_offset + center_value_offset + int(-1.0 * value_range_scale);
    }

    // write to byte frame

    frame[channel_1_hi] = SerialHelper::hiByte(ch1);
    frame[channel_1_lo] = SerialHelper::loByte(ch1);

    frame[channel_2_hi] = SerialHelper::hiByte(ch2);
    frame[channel_2_lo] = SerialHelper::loByte(ch2);

    frame[channel_3_hi] = SerialHelper::hiByte(ch3);
    frame[channel_3_lo] = SerialHelper::loByte(ch3);

    frame[channel_4_hi] = SerialHelper::hiByte(ch4);
    frame[channel_4_lo] = SerialHelper::loByte(ch4);

    frame[channel_5_hi] = SerialHelper::hiByte(ch5);
    frame[channel_5_lo] = SerialHelper::loByte(ch5);

    frame[channel_6_hi] = SerialHelper::hiByte(ch6);
    frame[channel_6_lo] = SerialHelper::loByte(ch6);

    // write to serial port
    this->writeFrame(reinterpret_cast<const char *>(frame), (size_t) this->command_length);
}
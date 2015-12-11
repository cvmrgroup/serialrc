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
 *       @date:   03.06.2014
 *****************************************************/

#include "artp.h"

ArTP::ArTP(const std::string name, std::string serial, boost::shared_ptr<boost::asio::io_service> io_service) :
        ArXX(name, maxRadios, serial, io_service)
{
    for (int id = 0; id < n_tx; id++)
    {
        frame[id * command_length + ch1_hi] = SerialHelper::hiByte(ch1_init_value);
        frame[id * command_length + ch1_lo] = SerialHelper::loByte(ch1_init_value);

        frame[id * command_length + ch2_hi] = SerialHelper::hiByte(ch2_init_value);
        frame[id * command_length + ch2_lo] = SerialHelper::loByte(ch2_init_value);

        frame[id * command_length + ch3_hi] = SerialHelper::hiByte(ch3_init_value);
        frame[id * command_length + ch3_lo] = SerialHelper::loByte(ch3_init_value);

        frame[id * command_length + ch4_hi] = SerialHelper::hiByte(ch4_init_value);
        frame[id * command_length + ch4_lo] = SerialHelper::loByte(ch4_init_value);

        frame[id * command_length + ch5_hi] = SerialHelper::hiByte(ch5_init_value);
        frame[id * command_length + ch5_lo] = SerialHelper::loByte(ch5_init_value);

        frame[id * command_length + ch6_hi] = SerialHelper::hiByte(ch6_init_value);
        frame[id * command_length + ch6_lo] = SerialHelper::loByte(ch6_init_value);
    }
}

ArTP::~ArTP()
{
}

void ArTP::onData(const char *frame, size_t length)
{
    // check if same data read
    if (length > 0)
    {
        // check if an XON was received
        if (frame[0] == XON)
        {
            // write data to andoinu
            this->writeData();
        } else
        {
            BOOST_LOG_TRIVIAL(error) << "no XON received on serial port [ " << this->getSerialNumber() << " ]";
        }
    }
}

void ArTP::writeData()
{
    for (auto entry: this->radios)
    {
        AbstractRadio *radio = entry.second;

        // get the transmitter id as string
        std::string txIdStr = radio->getTxId();
        // cast the id to int
        int id = boost::lexical_cast<int>(txIdStr);

        int ch1 = center_value_offset + radio->getThrottle() * value_range_scale;
        int ch2 = center_value_offset + radio->getRoll() * value_range_scale;
        int ch3 = center_value_offset + radio->getPitch() * value_range_scale;
        int ch4 = center_value_offset + radio->getYaw() * value_range_scale;
        int ch5 = center_value_offset + radio->getCh5() * value_range_scale;
        int ch6 = center_value_offset + radio->getCh6() * value_range_scale;

        // overwrite throttle signal if copter is supended
        if (radio->isSuspended())
        {
            ch1 = center_value_offset + -1.0 * value_range_scale;
        }

        // write to byte frame

        frame[id * command_length + ch1_hi] = SerialHelper::hiByte(ch1);
        frame[id * command_length + ch1_lo] = SerialHelper::loByte(ch1);

        frame[id * command_length + ch2_hi] = SerialHelper::hiByte(ch2);
        frame[id * command_length + ch2_lo] = SerialHelper::loByte(ch2);

        frame[id * command_length + ch3_hi] = SerialHelper::hiByte(ch3);
        frame[id * command_length + ch3_lo] = SerialHelper::loByte(ch3);

        frame[id * command_length + ch4_hi] = SerialHelper::hiByte(ch4);
        frame[id * command_length + ch4_lo] = SerialHelper::loByte(ch4);

        frame[id * command_length + ch5_hi] = SerialHelper::hiByte(ch5);
        frame[id * command_length + ch5_lo] = SerialHelper::loByte(ch5);

        frame[id * command_length + ch6_hi] = SerialHelper::hiByte(ch6);
        frame[id * command_length + ch6_lo] = SerialHelper::loByte(ch6);
    }

    // write to serial port
    this->writeFrame(reinterpret_cast<const char *>(this->frame), this->n_tx * this->command_length);
}

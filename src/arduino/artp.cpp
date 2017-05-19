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

#include <iostream>
#include "artp.h"

ArTP::ArTP(const std::string name, std::string serial,
           boost::shared_ptr<boost::asio::io_service> io_service) :
        ArXX(name, ATP_N_RADIOS, serial, io_service)
{
    this->first = true;

    for (int i = 0; i < ATP_N_RADIOS; i++)
    {
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH1_HI] = SerialHelper::hiByte(ch1_init_value);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH1_LO] = SerialHelper::loByte(ch1_init_value);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH2_HI] = SerialHelper::hiByte(ch2_init_value);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH2_LO] = SerialHelper::loByte(ch2_init_value);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH3_HI] = SerialHelper::hiByte(ch3_init_value);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH3_LO] = SerialHelper::loByte(ch3_init_value);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH4_HI] = SerialHelper::hiByte(ch4_init_value);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH4_LO] = SerialHelper::loByte(ch4_init_value);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH5_HI] = SerialHelper::hiByte(ch5_init_value);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH5_LO] = SerialHelper::loByte(ch5_init_value);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH6_HI] = SerialHelper::hiByte(ch6_init_value);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH6_LO] = SerialHelper::loByte(ch6_init_value);
    }
}

ArTP::~ArTP()
{
}

void ArTP::onData(const char *frame, size_t length)
{
    if (this->first)
    {
        BOOST_LOG_TRIVIAL(info) << "First serial message ignored successfully [ " << std::string(frame) << " ].";
        this->first = false;
    }
    else if (length == 1 && frame[0] == AXX_DELIMITER)
    {
        // cool
    }
    else
    {
        std::string ex = boost::str(boost::format("Received frame is not of expected type, S/N [ %1% ], message [ %2% ].") % this->getSerialNumber() % std::string(frame));
        BOOST_LOG_TRIVIAL(info) << ex;
        throw RadioException(ex);
    }

    this->writeData();
}

void ArTP::writeData()
{
    for (auto entry: this->modules)
    {
        AbstractTxModule *module = entry.second;

        // get the transmitter id as string and cast to int
        std::string txIdStr = module->getModuleId();
        int id = boost::lexical_cast<int>(txIdStr);

        int ch1 = this->center_value_offset + int(module->getThrottle() * this->value_range_scale);
        int ch2 = this->center_value_offset + int(module->getRoll() * this->value_range_scale);
        int ch3 = this->center_value_offset + int(module->getPitch() * this->value_range_scale);
        int ch4 = this->center_value_offset + int(module->getYaw() * this->value_range_scale);
        int ch5 = this->center_value_offset + int(module->getGear() * this->value_range_scale);
        int ch6 = this->center_value_offset + int(module->getAux1() * this->value_range_scale);

        std::cout << id << ": " << ch1 << " " << ch2 << " " << ch3 << " " << ch4 << " " << ch5 << " " << ch6 << std::endl;

        // write to byte frame
        this->frame[id * ATP_COMMAND_LENGTH + ATP_CH1_HI] = SerialHelper::hiByte(ch1);
        this->frame[id * ATP_COMMAND_LENGTH + ATP_CH1_LO] = SerialHelper::loByte(ch1);
        this->frame[id * ATP_COMMAND_LENGTH + ATP_CH2_HI] = SerialHelper::hiByte(ch2);
        this->frame[id * ATP_COMMAND_LENGTH + ATP_CH2_LO] = SerialHelper::loByte(ch2);
        this->frame[id * ATP_COMMAND_LENGTH + ATP_CH3_HI] = SerialHelper::hiByte(ch3);
        this->frame[id * ATP_COMMAND_LENGTH + ATP_CH3_LO] = SerialHelper::loByte(ch3);
        this->frame[id * ATP_COMMAND_LENGTH + ATP_CH4_HI] = SerialHelper::hiByte(ch4);
        this->frame[id * ATP_COMMAND_LENGTH + ATP_CH4_LO] = SerialHelper::loByte(ch4);
        this->frame[id * ATP_COMMAND_LENGTH + ATP_CH5_HI] = SerialHelper::hiByte(ch5);
        this->frame[id * ATP_COMMAND_LENGTH + ATP_CH5_LO] = SerialHelper::loByte(ch5);
        this->frame[id * ATP_COMMAND_LENGTH + ATP_CH6_HI] = SerialHelper::hiByte(ch6);
        this->frame[id * ATP_COMMAND_LENGTH + ATP_CH6_LO] = SerialHelper::loByte(ch6);
    }

    // write to serial port
    this->writeFrame(reinterpret_cast<const char *>(this->frame), (size_t) (ATP_FRAME_LENGTH));
}

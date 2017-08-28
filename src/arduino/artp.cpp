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

ArTP::ArTP(const std::string name, std::string serial,
           boost::shared_ptr<boost::asio::io_service> io_service) :
        ArXX(name, ATP_N_RADIOS, serial, io_service)
{
    this->first = true;

    int low = atp_center_value_offset - atp_value_range_scale;
    int neutral = atp_center_value_offset;

    for (int i = 0; i < ATP_N_RADIOS; i++)
    {
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH1_HI] = SerialHelper::hiByte(low);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH1_LO] = SerialHelper::loByte(low);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH2_HI] = SerialHelper::hiByte(neutral);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH2_LO] = SerialHelper::loByte(neutral);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH3_HI] = SerialHelper::hiByte(neutral);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH3_LO] = SerialHelper::loByte(neutral);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH4_HI] = SerialHelper::hiByte(neutral);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH4_LO] = SerialHelper::loByte(neutral);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH5_HI] = SerialHelper::hiByte(neutral);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH5_LO] = SerialHelper::loByte(neutral);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH6_HI] = SerialHelper::hiByte(neutral);
        this->frame[i * ATP_COMMAND_LENGTH + ATP_CH6_LO] = SerialHelper::loByte(neutral);
    }
}

void ArTP::addTxModule(AbstractTxModule *txModule)
{
    // this is a dsmx module
    DSMXModule *module = static_cast<DSMXModule *>(txModule);

    // add the txModule config to our map of configs
    RadioConfig config = module->getRadioConfig();
    int tx = boost::lexical_cast<int>(config.txId);

    int ch1 = atp_center_value_offset + int(module->getThrottle() * atp_value_range_scale);
    int ch2 = atp_center_value_offset + int(module->getRoll() * atp_value_range_scale);
    int ch3 = atp_center_value_offset + int(module->getPitch() * atp_value_range_scale);
    int ch4 = atp_center_value_offset + int(module->getYaw() * atp_value_range_scale);
    int ch5 = atp_center_value_offset + int(module->getGear() * atp_value_range_scale);
    int ch6 = atp_center_value_offset + int(module->getAux1() * atp_value_range_scale);

    this->frame[tx * ATP_COMMAND_LENGTH + ATP_CH1_HI] = SerialHelper::hiByte(ch1);
    this->frame[tx * ATP_COMMAND_LENGTH + ATP_CH1_LO] = SerialHelper::loByte(ch1);
    this->frame[tx * ATP_COMMAND_LENGTH + ATP_CH2_HI] = SerialHelper::hiByte(ch2);
    this->frame[tx * ATP_COMMAND_LENGTH + ATP_CH2_LO] = SerialHelper::loByte(ch2);
    this->frame[tx * ATP_COMMAND_LENGTH + ATP_CH3_HI] = SerialHelper::hiByte(ch3);
    this->frame[tx * ATP_COMMAND_LENGTH + ATP_CH3_LO] = SerialHelper::loByte(ch3);
    this->frame[tx * ATP_COMMAND_LENGTH + ATP_CH4_HI] = SerialHelper::hiByte(ch4);
    this->frame[tx * ATP_COMMAND_LENGTH + ATP_CH4_LO] = SerialHelper::loByte(ch4);
    this->frame[tx * ATP_COMMAND_LENGTH + ATP_CH5_HI] = SerialHelper::hiByte(ch5);
    this->frame[tx * ATP_COMMAND_LENGTH + ATP_CH5_LO] = SerialHelper::loByte(ch5);
    this->frame[tx * ATP_COMMAND_LENGTH + ATP_CH6_HI] = SerialHelper::hiByte(ch6);
    this->frame[tx * ATP_COMMAND_LENGTH + ATP_CH6_LO] = SerialHelper::loByte(ch6);

    // notify ArXX about add Radio
    ArXX::addTxModule(txModule);
}

void ArTP::onData(std::string frame)
{
    if (this->first)
    {
        BOOST_LOG_TRIVIAL(info) << "First serial message ignored successfully [ " << frame << " ].";
        this->first = false;
    }
    else if (frame[0] == AXX_DELIMITER)
    {
        // cool
        //BOOST_LOG_TRIVIAL(info) << "Correct frame received";
    }
    else
    {
        std::string msg = boost::str(boost::format("Received frame is not of expected type, name [ %1% ], message [ %2% ].") % this->getName() % std::string(frame));
        throw std::runtime_error(msg);
    }

    this->writeData();
}

void ArTP::writeData()
{
    for (auto const entry: this->modules)
    {
        AbstractTxModule *module = entry.second;

        // get the transmitter id as string and cast to int
        std::string txIdStr = module->getModuleId();
        int id = boost::lexical_cast<int>(txIdStr);

        int ch1 = atp_center_value_offset + int(module->getThrottle() * atp_value_range_scale);
        int ch2 = atp_center_value_offset + int(module->getRoll() * atp_value_range_scale);
        int ch3 = atp_center_value_offset + int(module->getPitch() * atp_value_range_scale);
        int ch4 = atp_center_value_offset + int(module->getYaw() * atp_value_range_scale);
        int ch5 = atp_center_value_offset + int(module->getGear() * atp_value_range_scale);
        int ch6 = atp_center_value_offset + int(module->getAux1() * atp_value_range_scale);

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

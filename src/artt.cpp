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

using namespace cv;
using namespace std;

ArTT::ArTT(string serial) :
ArXX(maxRadios, serial)
{
    token = 0;
}

ArTT::~ArTT()
{
}

void ArTT::readData(boost::system::error_code ec, size_t bytes)
{
    if(ec)
    {
        BOOST_LOG_TRIVIAL(error) << ec.message();
        return;
    }
    
    std::istream in(&buf);
    
    char c;
    in.read(reinterpret_cast<char *>(&c), sizeof(c));
    
    if (c == XON)
    {
        token = (token+1) % radios.size();
        writeData(token);
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "no XON received.";
        return;
    }
    
    boost::asio::async_read_until(*serialPort, buf, XON, boost::bind(&ArXX::readData, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void ArTT::writeData(unsigned int id)
{
    AbstractRadio *radio = radios[id];
    
    unsigned char frame[command_length];
    
    frame[header_1] = radio->isBinding() ? header_1_bind_mode : header_1_default;
    frame[relay_state] = radio->isEnabled() ? relay_state_on : relay_state_off;
    frame[tx_id] = (unsigned char)id;
    
    int ch1 = ch1_offset + center_value_offset + radio->getThrottle() * value_range_scale;
    int ch2 = ch2_offset + center_value_offset + radio->getRoll() * value_range_scale;
    int ch3 = ch3_offset + center_value_offset + radio->getPitch() * value_range_scale;
    int ch4 = ch4_offset + center_value_offset + radio->getYaw() * value_range_scale;
    int ch5 = ch5_offset + center_value_offset + radio->getCh5() * value_range_scale;
    int ch6 = ch6_offset + center_value_offset + radio->getCh6() * value_range_scale;
    
    // overwrite throttle signal if copter is supended
    if (radio->isSuspended())
    {
        ch1 = ch1_offset + center_value_offset + -1.0 * value_range_scale;
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
    boost::asio::async_write(*serialPort, boost::asio::buffer(frame, command_length), boost::bind(&ArXX::handle_write, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void ArTT::getRelayStates(vector<bool>& enabled)
{
    for(vector<AbstractRadio*>::iterator it = radios.begin(); it != radios.end(); it++)
    {
        enabled.push_back((*it)->isSuspended());
    }
}

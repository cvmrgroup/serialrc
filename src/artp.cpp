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

using namespace cv;
using namespace std;

ArTP::ArTP(QString serial) :
ArXX(maxRadios, serial)
{
    for(int id = 0; id < n_tx; id++)
    {    
        frame[id*command_length + ch1_hi] = SerialHelper::hiByte(ch1_init_value);
        frame[id*command_length + ch1_lo] = SerialHelper::loByte(ch1_init_value);
        
        frame[id*command_length + ch2_hi] = SerialHelper::hiByte(ch2_init_value);
        frame[id*command_length + ch2_lo] = SerialHelper::loByte(ch2_init_value);
        
        frame[id*command_length + ch3_hi] = SerialHelper::hiByte(ch3_init_value);
        frame[id*command_length + ch3_lo] = SerialHelper::loByte(ch3_init_value);
        
        frame[id*command_length + ch4_hi] = SerialHelper::hiByte(ch4_init_value);
        frame[id*command_length + ch4_lo] = SerialHelper::loByte(ch4_init_value);

        frame[id*command_length + ch5_hi] = SerialHelper::hiByte(ch5_init_value);
        frame[id*command_length + ch5_lo] = SerialHelper::loByte(ch5_init_value);
        
        frame[id*command_length + ch6_hi] = SerialHelper::hiByte(ch6_init_value);
        frame[id*command_length + ch6_lo] = SerialHelper::loByte(ch6_init_value);
    }
}

ArTP::~ArTP()
{
}

void ArTP::readData()
{
    char c;
    int length = serial->read(&c, 1);
    
    if (length > 0 && c == XON)
    {
        writeData();
    }
}

void ArTP::writeData(unsigned int id)
{
    for(vector<AbstractRadio*>::iterator it = radios.begin(); it != radios.end(); it++)
    {
        AbstractRadio *radio = *it;
        
        int id = radio->getTxId();
        
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
        
        frame[id*command_length + ch1_hi] = SerialHelper::hiByte(ch1);
        frame[id*command_length + ch1_lo] = SerialHelper::loByte(ch1);

        frame[id*command_length + ch2_hi] = SerialHelper::hiByte(ch2);
        frame[id*command_length + ch2_lo] = SerialHelper::loByte(ch2);

        frame[id*command_length + ch3_hi] = SerialHelper::hiByte(ch3);
        frame[id*command_length + ch3_lo] = SerialHelper::loByte(ch3);

        frame[id*command_length + ch4_hi] = SerialHelper::hiByte(ch4);
        frame[id*command_length + ch4_lo] = SerialHelper::loByte(ch4);

        frame[id*command_length + ch5_hi] = SerialHelper::hiByte(ch5);
        frame[id*command_length + ch5_lo] = SerialHelper::loByte(ch5);

        frame[id*command_length + ch6_hi] = SerialHelper::hiByte(ch6);
        frame[id*command_length + ch6_lo] = SerialHelper::loByte(ch6);      
    }
    
    // convert to qt byte array and write to serial port
    QByteArray databuf = QByteArray((char *)frame, n_tx*command_length);
    serial->write(databuf);
}

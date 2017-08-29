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

#ifndef ICARUS_SERIALHELPER_H
#define ICARUS_SERIALHELPER_H

/**
 * Serial helper class to calculate or join low and high bytes.
 */
struct SerialHelper
{
    static unsigned char loByte(int value)
    {
        return (value >> 8) & 0x00FF;
    }

    static unsigned char hiByte(int value)
    {
        return value & 0x00FF;
    }

    static int joinHiLoBytes(unsigned char hi, unsigned char lo)
    {
        return (hi << 8) | lo;
    }
};

#endif //ICARUS_SERIALHELPER_H

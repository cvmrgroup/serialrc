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

#ifndef ARTP_H
#define ARTP_H

#include "arxx.h"
#include "atpdefinitions.h"

/**
 * Software-side class for communication with the software running on the
 * tx3 built-in arduino.
 * This class implements a QThread and runs an infinite loop receiving a
 * send signal from the arduino and answering with a new copter command
 * if a new command was set.
 */
class ArTP : public ArXX
{

public:

    /// Constructor.
    ArTP(const std::string name, std::string serial,
         boost::shared_ptr<boost::asio::io_service> io_service);

    /// Destructor.
    virtual ~ArTP();

private:

    void onData(const char *frame, size_t length);

    /**
     *  write data to serial port
     */
    void writeData();

    /// offset from channel offset to center value
    static const int center_value_offset = 1115;
    /// scale from normalized values -1...1 to trainer port values 700...1530
    static const int value_range_scale = 0x19F;

    bool first;

    unsigned char frame[ATP_FRAME_LENGTH];

    static const int ch1_init_value = 0x2BC; // extern signal
    static const int ch2_init_value = 0x45b; // extern signal
    static const int ch3_init_value = 0x45b; // extern signal
    static const int ch4_init_value = 0x45b; // extern signal
    static const int ch5_init_value = 0x5fa; // extern signal
    static const int ch6_init_value = 0x45b; // extern signal
};

#endif /* ARTP_H */

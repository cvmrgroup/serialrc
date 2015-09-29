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

/**
 * Software-side class for communication with the software running on the
 * tx3 built-in arduino.
 * This class implements a QThread and runs an infinite loop receiving a
 * send signal from the arduino and answering with a new copter command
 * if a new command was set.
 */
class ArTP: public ArXX
{
    
public:
    
    /// Constructor.
    ArTP(const std::string name, std::string serial, boost::shared_ptr<boost::asio::io_service> io_service);
    
    /// Destructor.
    virtual ~ArTP();

private:

    void onData(const char *frame, size_t length);

    /**
     *  write data to serial port
     */
    void writeData();
    
    static const int maxRadios = 5;
    
    static const int n_tx = 5;
    static const int command_length = 12;
    
    static const int ch1_hi = 0;
    static const int ch1_lo = 1;
    static const int ch2_hi = 2;
    static const int ch2_lo = 3;
    static const int ch3_hi = 4;
    static const int ch3_lo = 5;
    static const int ch4_hi = 6;
    static const int ch4_lo = 7;
    static const int ch5_hi = 8;
    static const int ch5_lo = 9;
    static const int ch6_hi = 10;
    static const int ch6_lo = 11;
    
    /// offset from channel offset to center value
    static const int center_value_offset = 1115;
    /// scale from normalized values -1...1 to serial dsm values 700...1530
    static const int value_range_scale = 0x19F;
    
    static const int ch1_init_value = 0x2BC; // extern signal
    static const int ch2_init_value = 0x45b;	// extern signal
    static const int ch3_init_value = 0x45b;	// extern signal
    static const int ch4_init_value = 0x45b;	// extern signal
    static const int ch5_init_value = 0x5fa;	// extern signal
    static const int ch6_init_value = 0x45b;	// extern signal
    
    static const int ch6_full_range = 0x1754;
    
    /// Frame with copter commands sent to arduino via serial connection.
    unsigned char frame[n_tx * command_length];
};

#endif /* ARTP_H */

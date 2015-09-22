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

#ifndef ARTT_H
#define ARTT_H

#include "arxx.h"

/**
 * @brief Software-side class for communication with the software running on the
 * arduino triple transmission built-in arduino board.
 */
class ArTT: public ArXX
{
    Q_OBJECT
    
public:
    
    /**
     * @brief Constructor.
     */
    ArTT(QString serial);
    
    /**
     * @brief Destructor.
     */
    virtual ~ArTT();
    
    void getRelayStates(std::vector<bool>& enabled);
    
public slots:
    
    void readData();
    
private:
    
    /**
     *  write data to serial port
     */
    void writeData(unsigned int id = 0);
    
    static const int maxRadios = 2;
    
    /// length of a single transmitter command
    static const int command_length = 16;
    // number of channels
    static const int n_channels = 6;
    
    /// channel ids
    static const int header_1 = 0;
    static const int header_2 = 1;
    static const int channel_1_hi =  2;
    static const int channel_1_lo =  3;
    static const int channel_2_hi =  4;
    static const int channel_2_lo =  5;
    static const int channel_3_hi =  6;
    static const int channel_3_lo =  7;
    static const int channel_4_hi =  8;
    static const int channel_4_lo =  9;
    static const int channel_5_hi = 10;
    static const int channel_5_lo = 11;
    static const int channel_6_hi = 12;
    static const int channel_6_lo = 13;
    static const int relay_state = 14;
    static const int tx_id = 15;
    
    static const int ch1 = 0;
    static const int ch2 = 1;
    static const int ch3 = 2;
    static const int ch4 = 3;
    static const int ch5 = 4;
    static const int ch6 = 5;
    
    /// default header value
    static const int header_1_default = 0x0058;
    /// header value when in bind mode
    static const int header_1_bind_mode = 0x00D8;
    /// second header byte always zero
    static const int header_2_default = 0x0000;
    
    /// channel offsets
    static const int ch1_offset = 0x0000;
    static const int ch2_offset = 0x0400;
    static const int ch3_offset = 0x0800;
    static const int ch4_offset = 0x0C00;
    static const int ch5_offset = 0x1000;
    static const int ch6_offset = 0x1400;
    
    /// offset from channel offset to center value
    static const int center_value_offset = 0x01FF;
    /// scale from normalized values -1...1 to serial dsm values 170...852
    static const int value_range_scale = 0x0155;
    
    // multiwii trimming for super-x
    //#define CENTER_VALUE_OFFSET 0x1e2
    //#define VALUE_RANGE_SCALE   0x1e2
    
    /// relay values (relays are active low)
    static const char relay_state_on = 0;
    static const char relay_state_off = 1;
    
    /// Remember whos turn is next.
    unsigned int token;
};

#endif /* ARTT_H */

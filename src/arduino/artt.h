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

#include <vector>

#include "dsmxconstants.h"
#include "arxx.h"

/**
 * @brief Software-side class for communication with the software running on the
 * arduino triple transmission built-in arduino board.
 */
class ArTT : public ArXX
{

public:

    /**
     * @brief Constructor.
     */
    ArTT(const std::string name, std::string serial, boost::shared_ptr<boost::asio::io_service> io_service);

    /**
     * overwrite the addRadio for getting the transmitter ids
     * @param the AbstractRadio to add to this transmitter
     */
    void addRadio(AbstractRadio *radio);

private:

    void onData(const char *frame, size_t length);

    /**
     *  write data to serial port
     */
    void writeData(unsigned int id = 0);

    static const int maxRadios = 3;

    /// length of a single transmitter command
    static const int command_length = 16;
    // number of channels
    static const int n_channels = 6;

    /// channel ids
    static const int header_1 = 0;
    static const int header_2 = 1;
    static const int channel_1_hi = 2;
    static const int channel_1_lo = 3;
    static const int channel_2_hi = 4;
    static const int channel_2_lo = 5;
    static const int channel_3_hi = 6;
    static const int channel_3_lo = 7;
    static const int channel_4_hi = 8;
    static const int channel_4_lo = 9;
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

    // multiwii trimming for super-x
    //#define CENTER_VALUE_OFFSET 0x1e2
    //#define VALUE_RANGE_SCALE   0x1e2

    /// relay values (relays are active low)
    static const char relay_state_on = 0;
    static const char relay_state_off = 1;

    /// Remember whos turn is next.
    unsigned int token;

    std::vector<unsigned int> transmitterIds;
};

#endif /* ARTT_H */

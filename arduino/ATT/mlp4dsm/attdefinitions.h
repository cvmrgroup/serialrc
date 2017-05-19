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
/** @copyright    Hochschule RheinMain,
 *                University of Applied Sciences
 *     @author    Marc Lieser
 *    @version    1.0
 *       @date    15.05.2017
*****************************************************/

#ifndef ICARUS_ATTDEFINITIONS_H
#define ICARUS_ATTDEFINITIONS_H

/// Response delimiter
#define AXX_DELIMITER 0x3b

/// Byte frame length
#define ATT_FRAME_LENGTH 17

/// Some array positions
#define ATT_FRAME_TYPE 0
#define ATT_TX_ID 1
#define ATT_RELAY_STATE 2

/// Currently the only configuration
#define ATT_FRAME_RATE 3

/// Positions of signal bytes in frame
#define ATT_HEADER_1 3
#define ATT_HEADER_2 4
#define ATT_CH1_HI 5
#define ATT_CH1_LO 6
#define ATT_CH2_HI 7
#define ATT_CH2_LO 8
#define ATT_CH3_HI 9
#define ATT_CH3_LO 10
#define ATT_CH4_HI 11
#define ATT_CH4_LO 12
#define ATT_CH5_HI 13
#define ATT_CH5_LO 14
#define ATT_CH6_HI 15
#define ATT_CH6_LO 16

/// Maximum number of radios (number of hardware serials)
#define ATT_MAX_RADIOS 3

/// Frame types
#define ATT_FRAME_TYPE_CONFIG 1
#define ATT_FRAME_TYPE_SIGNAL 2

/// Relay values (relays are active low)
#define ATT_RELAY_STATE_ON 0
#define ATT_RELAY_STATE_OFF 1

#endif //ICARUS_ATTDEFINITIONS_H

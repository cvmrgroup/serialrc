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

#ifndef ICARUS_ATPDEFINITIONS_H
#define ICARUS_ATPDEFINITIONS_H

/// Response delimiter
#define AXX_DELIMITER 0x3b

#define ATP_FIRST_PIN 3

/// Maximum number of radios
#define ATP_N_RADIOS 5

/// Byte frame length
#define ATP_COMMAND_LENGTH 12

/// Frame length
#define ATP_FRAME_LENGTH (ATP_N_RADIOS*ATP_COMMAND_LENGTH)

/// Positions of signal bytes in frame
#define ATP_CH1_HI 0
#define ATP_CH1_LO 1
#define ATP_CH2_HI 2
#define ATP_CH2_LO 3
#define ATP_CH3_HI 4
#define ATP_CH3_LO 5
#define ATP_CH4_HI 6
#define ATP_CH4_LO 7
#define ATP_CH5_HI 8
#define ATP_CH5_LO 9
#define ATP_CH6_HI 10
#define ATP_CH6_LO 11

#endif //ICARUS_ATPDEFINITIONS_H

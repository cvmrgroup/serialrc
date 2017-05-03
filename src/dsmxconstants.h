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
 *       @date    03.05.2017
*****************************************************/

#ifndef DSMXCONSTANTS_H
#define DSMXCONSTANTS_H

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

#endif //DSMXCONSTANTS_H

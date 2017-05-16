/**
 *   #, #,         CCCCCC  VV    VV MM      MM RRRRRRR
 *  %  %(  #%%#   CC    CC VV    VV MMM    MMM RR    RR
 *  %    %## #    CC        V    V  MM M  M MM RR    RR
 *   ,%      %    CC        VV  VV  MM  MM  MM RRRRRR
 *   (%      %,   CC    CC   VVVV   MM      MM RR   RR
 *     #%    %*    CCCCCC     VV    MM      MM RR    RR
 *    .%    %/
 *       (%.      Computer Vision & Mixed Reality Group
 *
 *  @copyright:   Hochschule RheinMain,
 *                University of Applied Sciences
 *     @author:   Marc Lieser
 *    @version:   1.0
 *       @date:   13.05.2014
 */

#ifndef MLP4DSM_H
#define MLP4DSM_H

#include "Arduino.h"

// relay states (relays are active low!)
#define RLA_OFF 1
#define RLA_ON  0

// dmsx frame length protocol
#define DSM_FRAME_LENGTH 14

// baud rate defined by the dsm protocol
#define DSM_BAUD_RATE 125000

// class representing a mlp4dsm transmitter connected to the arduino due board
class MLP4DSM
{
public:

    // init transmitter
    void init(USARTClass *serial, int relayPIN);

    // set the frame rate (11000 or 22000 microseconds!)
    void setFrameRate(int frameRate);

    // turn on/off relay
    void setState(byte state);

    // set rc channels
    void setChannels(
            byte head1, byte head2,
            byte ch1Hi, byte ch1Lo,
            byte ch2Hi, byte ch2Lo,
            byte ch3Hi, byte ch3Lo,
            byte ch4Hi, byte ch4Lo,
            byte ch5Hi, byte ch5Lo,
            byte ch6Hi, byte ch6Lo);

    // transmit data via serial port to the receiver
    void send();

private:

    // the serial port the signals are sent on
    USARTClass *serial;

    // config data received
    bool initialized;

    // pin controlling the relay the transmitter is attached to
    int relayPIN;

    // current relay state
    byte relayState;

    // frame to be send
    byte frame[DSM_FRAME_LENGTH];

    // DSMX frame rate. 11000 or 22000 microseconds!
    unsigned long frameRate;

    // time stamp (microseconds) for the next pulse
    unsigned long nextPulse;
};

#endif /* MLP4DSM_H */

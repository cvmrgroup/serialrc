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

#include "mlp4dsm.h"

void MLP4DSM::init(USARTClass *_serial, int _relayPIN)
{
    serial = _serial;
    relayPIN = _relayPIN;

    initialized = false;

    // default frame rate
    frameRate = 22UL;

    // initialize pin so that relay is inactive at reset
    relayState = RLA_OFF;
    digitalWrite(relayPIN, relayState);
    // THEN set pin as outputs
    pinMode(relayPIN, OUTPUT);

    for (int i = 0; i < DSM_FRAME_LENGTH; i++)
    {
        frame[i] = 0;
    }

    serial->begin(DSM_BAUD_RATE);
}

void MLP4DSM::setFrameRate(int _frameRate)
{
    frameRate = (unsigned long) _frameRate * 1000;
    nextPulse = micros() + frameRate;
    initialized = true;
}

void MLP4DSM::setState(byte state)
{
    if (relayState != state)
    {
        // state changed
        digitalWrite(relayPIN, state);
        relayState = state;
    }
}

void MLP4DSM::setChannels(
        byte head1, byte head2,
        byte ch1Hi, byte ch1Lo,
        byte ch2Hi, byte ch2Lo,
        byte ch3Hi, byte ch3Lo,
        byte ch4Hi, byte ch4Lo,
        byte ch5Hi, byte ch5Lo,
        byte ch6Hi, byte ch6Lo)
{
    frame[0] = head1;
    frame[1] = head2;
    frame[2] = ch1Hi;
    frame[3] = ch1Lo;
    frame[4] = ch2Hi;
    frame[5] = ch2Lo;
    frame[6] = ch3Hi;
    frame[7] = ch3Lo;
    frame[8] = ch4Hi;
    frame[9] = ch4Lo;
    frame[10] = ch5Hi;
    frame[11] = ch5Lo;
    frame[12] = ch6Hi;
    frame[13] = ch6Lo;
}

void MLP4DSM::send()
{
    if (!initialized)
    {
        return;
    }

    unsigned long now = micros();

    if (now > nextPulse)
    {
        nextPulse = now + frameRate;
        serial->write(frame, DSM_FRAME_LENGTH);
    }
}

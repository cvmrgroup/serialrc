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

	// initialize pin so that relay is inactive at reset
	relayState = RLA_OFF;
	digitalWrite(relayPIN, relayState);
	// THEN set pin as outputs
	pinMode(relayPIN, OUTPUT);

	for(int i = 0; i < DSM_FRAME_LENGTH; i++)
	{
		frame[i] = 0;
	}

	serial->begin(DSM_BAUD_RATE);

	timeout = millis();
}

void MLP4DSM::setState(byte state)
{
	if(relayState != state)
	{
		// state changed
		digitalWrite(relayPIN, state);
		relayState = state;
	}
}

void MLP4DSM::setChannels(
		byte head1, byte head2,
		byte hiCh1, byte loCh1,
		byte hiCh2, byte loCh2,
		byte hiCh3, byte loCh3,
		byte hiCh4, byte loCh4,
		byte hiCh5, byte loCh5,
		byte hiCh6, byte loCh6)
{
	frame[ 0] = head1;
	frame[ 1] = head2;

	frame[ 2] = hiCh1;
	frame[ 3] = loCh1;

	frame[ 4] = hiCh2;
	frame[ 5] = loCh2;

	frame[ 6] = hiCh3;
	frame[ 7] = loCh3;

	frame[ 8] = hiCh4;
	frame[ 9] = loCh4;

	frame[10] = hiCh5;
	frame[11] = loCh5;

	frame[12] = hiCh6;
	frame[13] = loCh6;
}

void MLP4DSM::send()
{
	unsigned long currentMillis = millis();

	if((currentMillis-timeout >= DSM_SEND_RATE))
	{
		timeout = currentMillis;
		serial->write(frame, DSM_FRAME_LENGTH);
	}
}

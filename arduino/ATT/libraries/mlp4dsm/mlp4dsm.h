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

// time between frames defined by the dsm protocol
#define DSM_SEND_RATE 22UL

// baud rate defined by the dsm protocol
#define DSM_BAUD_RATE 125000

// class representing a mlp4dsm transmitter connected to the arduino due board
class MLP4DSM
{

public:

	// init transmitter
	void init(USARTClass *serial, int relayPIN);

	// turn on/off relay
	void setState(byte state);

	// set rc channels
	void setChannels(
			byte head1, byte head2,
			byte hiCh1, byte loCh1,
			byte hiCh2, byte loCh2,
			byte hiCh3, byte loCh3,
			byte hiCh4, byte loCh4,
			byte hiCh5, byte loCh5,
			byte hiCh6, byte loCh6);

	// transmit data via serial port to the receiver
	void send();

private:

	// the serial port the signals are sent on
	USARTClass *serial;

	// pin controlling the relay the transmitter is attached to
	int relayPIN;

	// current relay state
	byte relayState;

	// frame to be send
	byte frame[DSM_FRAME_LENGTH];

	// timer (instead of delay to avoid blocking of other operations)
	unsigned long timeout;
	
};

#endif /* MLP4DSM_H */

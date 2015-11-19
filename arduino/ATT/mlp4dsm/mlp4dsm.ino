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

#include <mlp4dsm.h>

// pins connected to relays
#define RLA_1 53
#define RLA_2 51
#define RLA_3 49

// frame sizes
#define N_TX                 2
#define BYTE_TX_FRAME_LENGTH 16

// transmission code sent to host when
// arduino is ready to process next command
#define XON 0x11

bool waiting;

// transmitter
MLP4DSM tx[N_TX];

// function to read serial buffer until it is empty
void clearBuffer()
{
  while(Serial.available())
  {
    Serial.read();
  }  
}

// arduino setup function
void setup()
{
  waiting = true;
  
  tx[0].init(&Serial1, RLA_1);
  tx[1].init(&Serial2, RLA_2);
  tx[2].init(&Serial3, RLA_3);
    
  // inititialize serial connection to computer
  Serial.begin(115200);
  
  // clear buffer
  clearBuffer();
  
  delay(1000);
  Serial.write(XON);
}

// arduino loop function
void loop()
{  
  if(Serial.available() == BYTE_TX_FRAME_LENGTH)
  {
    waiting = false;

    byte rbuffer[BYTE_TX_FRAME_LENGTH];
    int bytesRead = Serial.readBytes((char*)rbuffer, BYTE_TX_FRAME_LENGTH);
    
    if(bytesRead == BYTE_TX_FRAME_LENGTH)
    {
      // get id
      int id = rbuffer[15];
      
      // set data in transmitter
      tx[id].setChannels(
          rbuffer[ 0], rbuffer[ 1],  // header bytes
          rbuffer[ 2], rbuffer[ 3],  // channel 1
          rbuffer[ 4], rbuffer[ 5],  // channel 2
          rbuffer[ 6], rbuffer[ 7],  // channel 3
          rbuffer[ 8], rbuffer[ 9],  // channel 4
          rbuffer[10], rbuffer[11],  // channel 5
          rbuffer[12], rbuffer[13]); // channel 6
      
      // switch transmitter on/off
      tx[id].setState(rbuffer[14]);
    }
    else
    {
      clearBuffer();
    }
  }
  
  if(!waiting)
  {
    Serial.write(XON);
    waiting = true;
  }
  
  // send data
  for(int id = 0; id < N_TX; id++)
  {
    tx[id].send();
  }
}


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

#include "attdefinitions.h"

// pins connected to relays
#define RLA_1 53
#define RLA_2 51
#define RLA_3 49

// error LED
#define ERROR_LED 13

// frame sizes
#define N_TX 3

// transmitter
MLP4DSM tx[N_TX];

// function to read serial buffer until it is empty
void clearBuffer()
{
    while (Serial.available())
    {
        Serial.read();
    }
}

// arduino setup function
void setup()
{
    tx[0].init(&Serial1, RLA_1);
    tx[1].init(&Serial2, RLA_2);
    tx[2].init(&Serial3, RLA_3);

    pinMode(12, OUTPUT);
    pinMode(ERROR_LED, OUTPUT);

    digitalWrite(12, LOW);
    digitalWrite(ERROR_LED, LOW);

    byte hi[3];
    hi[0] = (byte) 'h';
    hi[1] = (byte) 'i';
    hi[2] = AXX_DELIMITER;
    
    // inititialize serial connection to computer
    Serial.begin(115200);
    Serial.write(hi, 3);
}

// arduino loop function
void loop()
{
    if (Serial.available() == ATT_FRAME_LENGTH)
    {
        byte rbuffer[ATT_FRAME_LENGTH];
        int bytesRead = Serial.readBytes(rbuffer, ATT_FRAME_LENGTH);

        if (bytesRead == ATT_FRAME_LENGTH)
        {
            byte reply[3];
            reply[0] = (byte) '!';
            reply[1] = (byte) '?';
            reply[2] = AXX_DELIMITER;

            // get id
            byte bid = rbuffer[ATT_TX_ID];
            byte bftype = rbuffer[ATT_FRAME_TYPE];

            int id = (int) bid;
            int ftype = (int) bftype;

            if (ftype == ATT_FRAME_TYPE_CONFIG)
            {
                reply[0] = bid;
                reply[1] = bftype;
                
                int frameRate = (int) rbuffer[ATT_FRAME_RATE];
                tx[id].setFrameRate(frameRate);
            
                digitalWrite(12, HIGH);
            }
            else if (ftype == ATT_FRAME_TYPE_SIGNAL)
            {
                reply[0] = bid;
                reply[1] = bftype;
                
                // set data in transmitter
                tx[id].setChannels(
                        rbuffer[ATT_HEADER_1], rbuffer[ATT_HEADER_2],  // header bytes
                        rbuffer[ATT_CH1_HI], rbuffer[ATT_CH1_LO],  // channel 1
                        rbuffer[ATT_CH2_HI], rbuffer[ATT_CH2_LO],  // channel 2
                        rbuffer[ATT_CH3_HI], rbuffer[ATT_CH3_LO],  // channel 3
                        rbuffer[ATT_CH4_HI], rbuffer[ATT_CH4_LO],  // channel 4
                        rbuffer[ATT_CH5_HI], rbuffer[ATT_CH5_LO],  // channel 5
                        rbuffer[ATT_CH6_HI], rbuffer[ATT_CH6_LO]); // channel 6

                // switch transmitter on/off
                tx[id].setState(rbuffer[ATT_RELAY_STATE]);
            }

            Serial.write(reply, 3);
        }
        else
        {
            digitalWrite(ERROR_LED, HIGH);
            clearBuffer();
        }
    }

    // send data
    for (int id = 0; id < N_TX; id++)
    {
        tx[id].send();
    }
}


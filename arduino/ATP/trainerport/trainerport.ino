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
 *     @author:   Robert Brylka
 *    @version:   1.02
 *       @date:   26.08.2015
 *****************************************************/

/*
 *
 *     A    B       C       D       E    F      SYNC
 *  + +--+ +-----+ +-----+ +-----+ +--+ +--+ +--    --+
 *  | |  | |     | |     | |     | |  | |  | |        |
 *  | |  | |     | |     | |     | |  | |  | |        |
 *  | |  | |     | |     | |     | |  | |  | |        |
 *  +-+  +-+     +-+     +-+     +-+  +-+  +-+        +
 *   P    P       P       P       P    P    P
 *
 *  |---------------------- 22ms ---------------------|
 *
 *  A..F  PULSE  516-1710 us (at 150% max servo travel)
 *  P     PAUSE       400 us
 *
 *  SYNC  22ms - (|A|+|B|+|C|+|D|+|E|+|F|+7*|P|)
 *
 *  PINS  3..7
 *
 */

#include <DueTimer.h>

#include "atpdefinitions.h"

// error LED
#define ERROR_LED 53

/**
 * For 7 pauses and 6 channels at full servo travel we need at least
 *   7 * 400 + 6 * 1710 = 13060 bits
 * to encode any possible state of the signal.
 * During the SYNC phase the output is set to high.
 * Using an unsigned integer for the bitmask we at least need an array of size
 *   13060 / 32 = 408,125 --> 409
 */
#define MASK_LENGTH 409

// current signal state in micro seconds
int us = 0;

byte initBMBuffer[ATP_FRAME_LENGTH];

// allocate a byte array of the size MASK_LENGTH * 32, 
// because 5 Bits of a byte will be used by the 5 signals
byte bArray[MASK_LENGTH * 32] = {0};

// create
unsigned int bmCtr0[MASK_LENGTH] = {0};
unsigned int bmCtr1[MASK_LENGTH] = {0};
unsigned int bmCtr2[MASK_LENGTH] = {0};
unsigned int bmCtr3[MASK_LENGTH] = {0};
unsigned int bmCtr4[MASK_LENGTH] = {0};

unsigned int ones = 16;
unsigned int zeros = 16;
unsigned int idx = 0;

unsigned int *indexP = &idx;
unsigned int *zerosP = &zeros;
unsigned int *onesP = &ones;

// pointer to the registry set
volatile uint32_t *PIN_REG;

/**
 * The first change after the first pause is given by 400 us so the half of the
 * mask by idx 12 need to be ones.
 */
void initFixBmPart()
{
    bmCtr0[12] = ~(((1 << 16) - 1) << 16);
    bmCtr1[12] = ~(((1 << 16) - 1) << 16);
    bmCtr2[12] = ~(((1 << 16) - 1) << 16);
    bmCtr3[12] = ~(((1 << 16) - 1) << 16);
    bmCtr4[12] = ~(((1 << 16) - 1) << 16);
}

/**
 * Generate the neutral signal
 * throttle =  0%
 * roll     = 50%
 * pitch    = 50%
 * yaw      = 50%
 * gear     =  0%
 * aux1     =  0%
 */
void initialBMPart(byte *iBMBuffer)
{
    for (int i = 0; i < 5; i++)
    {
        iBMBuffer[i * 12 + 0] = B00000010;
        iBMBuffer[i * 12 + 1] = B10111100;

        iBMBuffer[i * 12 + 2] = B00000100;
        iBMBuffer[i * 12 + 3] = B01011011;

        iBMBuffer[i * 12 + 4] = B00000100;
        iBMBuffer[i * 12 + 5] = B01011011;

        iBMBuffer[i * 12 + 6] = B00000100;
        iBMBuffer[i * 12 + 7] = B01011011;

        iBMBuffer[i * 12 + 8] = B00000010;
        iBMBuffer[i * 12 + 9] = B10111100;

        iBMBuffer[i * 12 + 10] = B00000010;
        iBMBuffer[i * 12 + 11] = B10111100;
    }
}

/**
 * Copy the five int array into one byte array The acces to a byte array is
 * faster than to five diferent int array
 */
void fillByteArray()
{
    for (int i = 0; i < MASK_LENGTH; i++)
    {
        for (int k = 0; k < 32; k++)
        {
            byte tmp = 0;
            tmp |= ((bmCtr0[i] >> 31 - k) & 1) << 0;
            tmp |= ((bmCtr1[i] >> 31 - k) & 1) << 1;
            tmp |= ((bmCtr2[i] >> 31 - k) & 1) << 2;
            tmp |= ((bmCtr3[i] >> 31 - k) & 1) << 3;
            tmp |= ((bmCtr4[i] >> 31 - k) & 1) << 4;

            bArray[i * 32 + k] = tmp;
        }
    }
}

/**
 * Set the ZEROS and ONES of one channel to the chosen bitmask. Use memset as
 * set method.
 */
void setChannel(unsigned int *mask, short channel, unsigned int *index,
                unsigned int *zeros, unsigned int *ones, bool last = false)
{
    unsigned int chQuo = (channel - *ones) / 32;
    memset(mask + *index, 0xFF, chQuo * sizeof(int));
    *index += chQuo;
    *ones = (channel - *ones) % 32;
    *zeros = 32 - *ones;
    mask[*index] = ((1 << *ones) - 1) << *zeros;
    (*index)++;
    chQuo = (400 - *zeros) / 32;
    memset(mask + *index, 0, chQuo * sizeof(int));
    *index += chQuo;
    *zeros = (400 - *zeros) % 32;
    *ones = 32 - *zeros;
    mask[*index] = ~(((1 << *zeros) - 1) << *ones);
    (*index)++;
    if (last)
    {
        memset(mask + *index, 0xFF, (MASK_LENGTH - *index) * sizeof(int));
    }
}

/**
 * Call the setChannel(..) method to set the six channels of one int bitmask.
 */
void setCtr(unsigned int *mask, short ch1, short ch2, short ch3, short ch4,
            short ch5, short ch6)
{
    ones = 16;
    zeros = 16;
    idx = 13;
    setChannel(mask, ch1, indexP, zerosP, onesP);
    setChannel(mask, ch2, indexP, zerosP, onesP);
    setChannel(mask, ch3, indexP, zerosP, onesP);
    setChannel(mask, ch4, indexP, zerosP, onesP);
    setChannel(mask, ch5, indexP, zerosP, onesP);
    setChannel(mask, ch6, indexP, zerosP, onesP, true);
}

/**
 * Reads a byte buffer received from the USB programming port.
 * 1. Convert high and low bytes to original short values.
 * 2. Call the setCtr(..)
 * 3. Call the fillbyteArray() method to copy the five int bitmask to one byte
 *    array.
 */
void setCtrBitmasks(byte *rbuffer)
{
    for (int i = 0; i < 5; i++)
    {
        short ch1 = (short) ((rbuffer[i * 12 + 0] << 8) | rbuffer[i * 12 + 1]);
        short ch2 = (short) ((rbuffer[i * 12 + 2] << 8) | rbuffer[i * 12 + 3]);
        short ch3 = (short) ((rbuffer[i * 12 + 4] << 8) | rbuffer[i * 12 + 5]);
        short ch4 = (short) ((rbuffer[i * 12 + 6] << 8) | rbuffer[i * 12 + 7]);
        short ch5 = (short) ((rbuffer[i * 12 + 8] << 8) | rbuffer[i * 12 + 9]);
        short ch6 = (short) ((rbuffer[i * 12 + 10] << 8) | rbuffer[i * 12 + 11]);

        switch (i)
        {
            case 0:
                setCtr(bmCtr0, ch1, ch2, ch3, ch4, ch5, ch6);
                break;
            case 1:
                setCtr(bmCtr1, ch1, ch2, ch3, ch4, ch5, ch6);
                break;
            case 2:
                setCtr(bmCtr2, ch1, ch2, ch3, ch4, ch5, ch6);
                break;
            case 3:
                setCtr(bmCtr3, ch1, ch2, ch3, ch4, ch5, ch6);
                break;
            case 4:
                setCtr(bmCtr4, ch1, ch2, ch3, ch4, ch5, ch6);
                break;
            default:
                break;
        }
    }

    fillByteArray();
}

/**
 * Get the registry pointer and the registry masks. Pins 3 to 7 belong to same
 * registry pointer, so get any of them (e.g. 5).
 */
void initREG()
{
    PIN_REG = portOutputRegister(digitalPinToPort(5));
}

/**
 * If we get some buffer overflow we need to clear the serial buffer, so read
 * until it is empty.
 */
void clearBuffer()
{
    while (Serial.available())
    {
        Serial.read();
    }
}

/**
 * Arduinos setup method. Initialize pins, serial and libs.
 */
void setup()
{
    // set pins 3..7 as outputs
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);

    pinMode(ERROR_LED, OUTPUT);
    digitalWrite(ERROR_LED, LOW);

    // get registry pointer
    initREG();

    // init the default signal
    initFixBmPart();
    initialBMPart(initBMBuffer);
    setCtrBitmasks(initBMBuffer);

    // setup the serial port
    Serial.begin(115200);

    // initiate communication with remote pc
    Serial.write(AXX_DELIMITER);

    // setup the two interrupt timer
    Timer0.attachInterrupt(setDigOut).setPeriod(2);
    Timer1.attachInterrupt(startTimer0).setPeriod(22000);
    Timer1.start();
}

/**
 * Sets Pins 3 to 7 to high state
 * 0x37E00000 => 00110111111000000000000000000000
 */
void setDigOutToHighLevel()
{
    *PIN_REG = 0x37E00000;
}

/**
 * This function is called by the interrupt timer every 2us. Output pins 3..7
 * will be set to the current value of the bitmask bArray by given ms which
 * represent the elapsed time
 */
void setDigOut()
{
    if (us <= 13060)
    {
        unsigned int regTMP = 0x20000000;
        regTMP |= ((bArray[us] & 1) << 28);
        regTMP |= (((bArray[us] >> 1) & 1) << 26);
        regTMP |= (((bArray[us] >> 2) & 1) << 25);
        regTMP |= (((bArray[us] >> 3) & 1) << 24);
        regTMP |= (((bArray[us] >> 4) & 1) << 23);

        *PIN_REG = regTMP;
        us += 2;
    }
    else
    {
        Timer0.stop();
        setDigOutToHighLevel();
        us = 0;
        createNewSignalBitmask();
    }
}

/**
 * Starts timer 0.
 */
void startTimer0()
{
    Timer0.start();
}

/**
 * During the SYNC period the serial port will be checked for new data with
 * signal information. If new data has arrived, a new bitmask will be created by
 * calling the setCtrBitmask(..) method.
 */
void createNewSignalBitmask()
{
    if (Serial.available() == ATP_FRAME_LENGTH)
    {
        byte rbuffer[ATP_FRAME_LENGTH];
        int bytesRead = Serial.readBytes(rbuffer, ATP_FRAME_LENGTH);

        if (bytesRead == ATP_FRAME_LENGTH)
        {
            setCtrBitmasks(rbuffer);
            Serial.write(AXX_DELIMITER);
        }
        else
        {
            digitalWrite(ERROR_LED, HIGH);
            clearBuffer();
        }
    }
}

void loop()
{
}

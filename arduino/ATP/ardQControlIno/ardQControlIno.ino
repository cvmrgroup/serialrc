/*****************************************************/
/**		  Master Project	            **/
/**						    **/
/**	  Created by Robert Brylka                  **/
/**	  during the summer session of 2014         **/
/**						    **/
/**		version 1.01 // 26.08.15  	    **/
/**						    **/
/*****************************************************/

/**

       A     B        C        D        E     F        SYNC
   +  +--+  +-----+  +-----+  +-----+  +--+  +--+  +---    ------+
   |  |  |  |     |  |     |  |     |  |  |  |  |  |     	 |
   |  |  |  |     |  |     |  |     |  |  |  |  |  |    	 |
   |  |  |  |     |  |     |  |     |  |  |  |  |  |     	 |
   +--+  +--+     +--+     +--+     +--+  +--+  +--+     	 +
    P     P        P        P        P     P     P

 A...F	PULSE			700 - 1530 us
 P	PAUSE			400 us

 Channel:                       Description

 A - throttle			0%	- left stick
 B - roll			50%	- left stick
 C - pitch			50%	- right stick
 D - yaw			50%	- right stick
 E -				50%
 F - Fly Modus			- switch 0% / 50% / 100%

 SYNC				22ms - (|A|+|B|+|C|+|D|+|E|+|F|+7*|P|)
 
 
**/

#include <DueTimer.h>

/**
  For 7 Pause and 6 channel with full extent we need at least
  7 * 400 + 6 * 1530 = 11980 bits to encode any posible state of the signal
  during the SYNC phase the aoutput can just be set to high
  Using an unsigned integer for the bitmask we need at least an array of size 11980 / 32 = 374,375 --> 375
**/
#define MASK_LENGTH 375

// frame size -> 5 devices times 6 chanels (channel = two bytes)
#define BYTE_FRAME_LENGTH 5 * 6 * 2

// current signal state in micro seconds
int us = 0;

byte initBMBuffer[BYTE_FRAME_LENGTH];

#define XON 0x11

int bytesRead = 0;

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
*	The first change after the first pause is given
*	by 400 us so the half of the mask by idx 12
*	need to be ones
**/
void initFixBmPart()
{
    bmCtr0[12] = ~(((1 << 16) - 1) << 16);
    bmCtr1[12] = ~(((1 << 16) - 1) << 16);
    bmCtr2[12] = ~(((1 << 16) - 1) << 16);
    bmCtr3[12] = ~(((1 << 16) - 1) << 16);
    bmCtr4[12] = ~(((1 << 16) - 1) << 16);
}

/**
*	Generate the neutral signal where
*	throttle       = 0%
*	roll           = 50%
*	pitch          = 50%
*	yaw            = 50%
*	channel 5 & 6  = 50%
**/
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
        iBMBuffer[i * 12 + 8] = B00000100;
        iBMBuffer[i * 12 + 9] = B01011011;
        iBMBuffer[i * 12 + 10] = B00000100;
        iBMBuffer[i * 12 + 11] = B01011011;
    }
}

/**
*	Copy the five int array into one byte array
*	The acces to a byte array is faster than
*	to five diferent int array
**/
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
*	Set the ZEROS and ONES of one channel
*	to chosen bitmask. Use memset as set method.
**/
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
*	Call the setChannel(..) method to set
*	the six channel  of one int bitmask
**/
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
*	Reads the byte buffer received by comport
*	and set paired bytes to their primary meaning
*	of a short value. Call the setCtr(...)
*	At the end call the fillbyteArray() method
*	to copy the five int bitmask to one byte array
**/
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
*	Get the registry pointer and the registry masks
*	Pins 3 to 7 belongs to same registry pointer
*	so get any of them e.x. 5
**/
void initREG()
{
    PIN_REG = portOutputRegister(digitalPinToPort(5));
}

/**
*	If we get some buffer overflow we need to clear
*	the serial buffer, so read it until it is empty
**/
void clearBuffer()
{
    while (Serial.available())
    {
        Serial.read();
    }
}

void setup()
{
    // setup the serial port
    Serial.begin(115200);

    // set pins 3 .. 7 as outputs
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);

    // get registry pointer
    initREG();

    // init the default signal
    initFixBmPart();
    initialBMPart(initBMBuffer);
    setCtrBitmasks(initBMBuffer);

    // clear serial buffer
    clearBuffer();
    // send a o.k. to the remote pc,
    // and let the communication begin
    Serial.write(XON);

    // Setup the two interrupt timer
    Timer0.attachInterrupt(setDigOut).setPeriod(2);
    Timer1.attachInterrupt(startTimer0).setPeriod(22000);
    Timer1.start();
}

/**
*    Sets Pins 3 to 7 to high state
*    0x37E00000 => 00110111111000000000000000000000
**/
void setDigOutToHighLevel()
{
    *PIN_REG = 0x37E00000;
}

/**
*	This function is called by interrupt timer any 2 us
*	Output pins 3...7 will be set by current value
*	of the bitmask bArray by given ms which represent
*	the elapsed time
**/
void setDigOut()
{
    if (us <= 11980)
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
*	Starts timer 0
**/
void startTimer0()
{
    Timer0.start();
}

/**
*	During the SYNC period the serial port will be 
*	check for new data with signal information
*	If new data is arrived a new bitmask will be 
*	create by calling the setCtrBitmask(...) 
**/
void createNewSignalBitmask()
{
    if (Serial.available() == BYTE_FRAME_LENGTH)
    {
        byte rbuffer[BYTE_FRAME_LENGTH];
        bytesRead = Serial.readBytes(rbuffer, BYTE_FRAME_LENGTH);
        bool error = bytesRead != BYTE_FRAME_LENGTH;
        if (error)
        {
            clearBuffer();
        }
        else
        {
            setCtrBitmasks(rbuffer);
            Serial.write(XON);
        }
    }
}

void loop()
{
}

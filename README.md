# SerialRC

This project implements different approaches on radio remote controlling multirotors or fixed wing aircrafts.

1. Trainer Port: An Arduino DUE hooked up via serial connection to your computer can control up to five transmitters having trainer port jacks.
1. DSMX Module: Connect a bare DSMX transmitter module taken from a budget RC to an Arduino Due and control up to three modules (currently requires an additional relais board).
1. DSMX Module Pi: Connect a bare DSMX transmitter with a transistor directly to the GPIO pins of a Raspberry Pi.

**Infrastructure for compact aerial robots under supervision**

## Prerequisites

An Arduino development environment should be set up in order to ensure writing permissions to the according serial port.

This guide is for an installation on Mac OS X. Compilation should not be an issue under other operating systems.

Install required libraries using [homebrew](http://brew.sh):

    brew tap homebrew/science
	brew install cmake
	brew install boost

For the examples to built
    
    brew install sdl            # version 1.X
    
to enable gamepad support.

## Build

    mkdir build
    cd build
    cmake ..
    make
    
Building the bare DSMX transmitter module solution on a Raspberry Pi requires the additional library

* [wiringPi](https://projects.drogon.net/raspberry-pi/wiringpi/download-and-install/)

Building then requires to pass the cmake flag

    -DWITH_RASPBERRYPI=True

## Install

    sudo make install

## Example

Pass the cmake flag

    -DBUILD_EXAMPLE=True
    
If ```WITH_RASPBERRY``` is defined, the example will create the Raspberry Pi bare DSMX module approach with a default configuration.

If not, the trainer port solution with a more sophisticated configuration will be build.

The examples let the user control a quadrotor with a gamepad (only DualShock 3 and 4 are supported, but the implementation can be adjusted with a reasonable effort).

Gamepad buttons:
* __Square__: Toggle Transmitter on/off (bare DSMX module only)
* __Circle__: Bind the transmitter (bare DSMX module only)
* __Triangle__: Arm
* __Cross__: Disarm
* __L1__: Gear (Channel 5) 
* __L2__: Aux1 (Channel 6)

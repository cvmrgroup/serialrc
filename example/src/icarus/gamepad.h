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
 *     @author:   Marc Lieser
 *    @version:   1.0
 *       @date:   13.05.2014
 *****************************************************/

#ifndef ICARUS_GAMEPAD_H
#define ICARUS_GAMEPAD_H

#include <string>
#include <vector>
#include <iostream>

// SDL defines its own main function. This leads to problems using MS Windows.
#include "SDL.h"

#ifdef _WIN32
#undef main
#endif

#include "SDL_joystick.h"

/**
 * Sony DualShock 3 implementation for multicopter control and simulation.
 */
class Gamepad
{

public:

    Gamepad();

    virtual ~Gamepad();

    bool open(int device = 0);

    void close();

    std::string getDeviceName();

    bool update();

    std::vector<float> getAxisControls();

    bool buttonToggled(int buttonId);

    bool buttonPressed(int buttonId);

    bool inNeutralPosition();

    void printDeviceState();

private:

    int device;
    SDL_Joystick *joystick;

    int nButtons;
    int nAxes;
    std::string deviceName;

    short *axes;
    short *buttons;

    short *prevAxes;
    short *prevButtons;

    bool axisExceededThreshold(int axis);

    bool inputChanged;

    int axisSensitivityThreshold;
    int neutralPositionThreshold;
};

#endif //ICARUS_GAMEPAD_H


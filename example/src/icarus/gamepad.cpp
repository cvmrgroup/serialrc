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

#include "gamepad.h"

Gamepad::Gamepad()
{
    this->device = 0;

    this->nAxes = 0;
    this->nButtons = 0;

    this->axisSensitivityThreshold = 512;
    this->neutralPositionThreshold = 2048;

    this->inputChanged = false;

    this->buttons = nullptr;
    this->prevButtons = nullptr;
    this->axes = nullptr;
    this->prevAxes = nullptr;
}

Gamepad::~Gamepad()
{
    delete[] this->buttons;
    delete[] this->axes;
    delete[] this->prevButtons;
    delete[] this->prevAxes;
}

bool Gamepad::open(int device)
{
    this->device = device;

    int init = SDL_Init(SDL_INIT_JOYSTICK);
    if (init != 0)
    {
        std::cerr << "Initializing SDL joystick failed." << std::endl;
        return false;
    }

    int numJoysticks = SDL_NumJoysticks();

    std::cout << "Number of gamepads detected [ " << numJoysticks << " ]." << std::endl;

    if (this->device >= numJoysticks)
    {
        std::cerr << "No gamepad with index [ " << this->device << " ] connected." << std::endl;
        return false;
    }

    this->joystick = SDL_JoystickOpen(this->device);

    if (this->joystick == nullptr)
    {
        std::cerr << "Failed to open gamepad." << std::endl;
        return false;
    }

    const char *identifier = SDL_JoystickName(this->device);
    this->deviceName = std::string(identifier);

    this->nAxes = SDL_JoystickNumAxes(this->joystick);
    this->nButtons = SDL_JoystickNumButtons(this->joystick);

    SDL_JoystickEventState(SDL_IGNORE);

    this->buttons = new short[this->nButtons]{ 0 };
    this->prevButtons = new short[this->nButtons]{ 0 };

    this->axes = new short[this->nAxes]{ 0 };
    this->prevAxes = new short[this->nAxes]{ 0 };

    std::cout << "Gamepad [ " << this->deviceName << " ] opened." << std::endl;

    return true;
}

void Gamepad::close()
{
    if (SDL_JoystickOpened(this->device))
    {
        SDL_JoystickClose(this->joystick);
    }
}

bool Gamepad::buttonToggled(int id)
{
    return this->buttons[id] && !this->prevButtons[id];
}

bool Gamepad::buttonPressed(int id)
{
    return (bool) this->buttons[id];
}

bool Gamepad::inNeutralPosition()
{
    for (int i = 0; i < this->nAxes; i++)
    {
        if (abs(this->axes[i]) > this->neutralPositionThreshold)
        {
            return false;
        }
    }

    return true;
}

bool Gamepad::axisExceededThreshold(int axis)
{
    return abs(this->axes[axis] - this->prevAxes[axis]) > this->axisSensitivityThreshold;
}

void Gamepad::printDeviceState()
{
    for (int i = 0; i < this->nButtons; i++)
    {
        std::cout << i << ":" << this->buttons[i] << " ";
    }
    for (int i = 0; i < this->nAxes; i++)
    {
        std::cout << i << ":" << this->axes[i] << " ";
    }
    std::cout << std::endl;
}

bool Gamepad::update()
{
    if (!SDL_JoystickOpened(this->device))
    {
        return false;
    }

    if (this->inputChanged)
    {
        std::memcpy(this->prevButtons, this->buttons, this->nButtons * sizeof(short));
        std::memcpy(this->prevAxes, this->axes, this->nAxes * sizeof(short));
        this->inputChanged = false;
    }

    SDL_JoystickUpdate();

    for (int i = 0; i < this->nButtons; i++)
    {
        this->buttons[i] = SDL_JoystickGetButton(this->joystick, i);
    }

    for (int i = 0; i < this->nAxes; i++)
    {
        this->axes[i] = SDL_JoystickGetAxis(this->joystick, i);
    }

    // check if event is worth processing by filtering out gyro events and
    // checking if button/axes states changed since last iteration

    bool axesChanged = false;
    bool buttonsChanged = false;

    // range operation ignores gyro axes events
    for (int i = 0; i < this->nButtons; i++)
    {
        if (this->buttons[i] != this->prevButtons[i])
        {
            buttonsChanged = true;
            break;
        }
    }

    for (int i = 0; i < this->nAxes; i++)
    {
	if (i > 3)
	{
	    // ignore events from other axes than the 4 control levers
	    break;
	}
	
	if (this->axes[i] != this->prevAxes[i])
        {
            axesChanged = true;
            break;
        }
    }

    // refine axes changed
    bool thresholdExceeded = false;

    if (axesChanged)
    {
        for (int i = 0; i < this->nAxes; i++)
        {
            thresholdExceeded |= axisExceededThreshold(i);
        }
    }

    if (buttonsChanged || (axesChanged && thresholdExceeded))
    {
        this->inputChanged = true;
    }

    return this->inputChanged;
}

std::vector<float> Gamepad::getAxisControls()
{
    float max = 32768.0f;

    std::vector<float> controls;

    for (int i = 0; i < this->nAxes; i++)
    {
        // invert values and normalize axis
        // lower/left lever position: -1
        // upper/right lever position: 1.
        float norm = -this->axes[i] / max;
        controls.push_back(norm);
    }

    return controls;
}

std::string Gamepad::getDeviceName()
{
    return this->deviceName;
}

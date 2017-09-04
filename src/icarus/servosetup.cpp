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
/** @copyright    Hochschule RheinMain,
 *                University of Applied Sciences
 *     @author    Marc Lieser
 *    @version    1.0
 *       @date    16.05.2017
*****************************************************/

#include "servosetup.h"

ServoSetup::ServoSetup()
{
    this->name = "";
    this->travel = 1.0;
    this->switchPositions = 0;
    this->switchValue = 0.0;
    this->switchValues = nullptr;
}

ServoSetup::ServoSetup(std::string name, int travel, bool reversed, int switchPositions)
        : name(name)
{
    this->switchValues = nullptr;
    this->initTravel(travel, reversed);
    this->initSwitch(switchPositions);
}

ServoSetup::ServoSetup(const ServoSetup &other)
{
    this->name = other.name;
    this->travel = other.travel;
    this->switchValues = nullptr;
    this->switchValue = 0.0;
    this->initSwitch(other.switchPositions);
}

ServoSetup &ServoSetup::operator=(const ServoSetup &other)
{
    this->name = other.name;
    this->travel = other.travel;
    this->switchValues = nullptr;
    this->switchValue = 0.0;
    this->initSwitch(other.switchPositions);
    return *this;
}

ServoSetup::~ServoSetup()
{
    if (this->switchValues != nullptr)
    {
        delete[] this->switchValues;
    }
}

void ServoSetup::initTravel(int travel, bool reversed)
{
    this->travel = (double) travel / 100.;

    if (reversed)
    {
        this->travel = -this->travel;
    }
}

double ServoSetup::getServoTravel(double normalizedValue)
{
    return normalizedValue * this->travel;
}

void ServoSetup::initSwitch(int positions)
{
    if (2 <= positions && positions <= 3)
    {
        this->switchValues = new double[positions];

        this->switchPositions = positions;

        switch (positions)
        {
            case 2:
                this->switchValues[0] = -this->travel;
                this->switchValues[1] = this->travel;
                break;
            case 3:
                this->switchValues[0] = -this->travel;
                this->switchValues[1] = 0.0;
                this->switchValues[2] = this->travel;
                break;
            default:
                break;
        }

        // set initial values to what is switch position zero on a DX18 radio
        this->position = positions - 1;
        this->switchValue = this->switchValues[this->position];
    }
}

double ServoSetup::getSwitchValue()
{
    return this->switchValue;
}

double ServoSetup::nextPosition()
{
    if (this->switchPositions)
    {
        this->position = (this->position + 1) % this->switchPositions;
        this->switchValue = this->switchValues[this->position];
        return this->switchValue;
    }

    return 0.0;
}
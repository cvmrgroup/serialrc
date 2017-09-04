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
 *       @date    12.05.2017
*****************************************************/

#ifndef ICARUS_SERVOSETUP_H
#define ICARUS_SERVOSETUP_H

#include <string>

class ServoSetup
{
public:

    ServoSetup();

    /**
     * Full parameter constructor.
     * @param name The channel description.
     * @param travel The travel way in percent.
     * @param reverse Flag if channel should be reversed.
     * @param switchPositions The number of switch positions.
     */
    ServoSetup(std::string name, int travel, bool reversed, int switchPositions);

    ServoSetup(const ServoSetup &other);

    ServoSetup &operator=(const ServoSetup &other);

    virtual ~ServoSetup();

    void initTravel(int travel, bool reversed);

    double getServoTravel(double normalizedValue);

    void initSwitch(int positions);

    double getSwitchValue();

    double nextPosition();

    std::string name;
    double travel;

private:

    int switchPositions;
    int position;
    double switchValue;
    double *switchValues;
};

#endif //ICARUS_SERVOSETUP_H

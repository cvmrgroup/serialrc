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
 *       @date    29.08.2017
*****************************************************/

#ifndef QUADCOPTIC_CONTROLINPUT_H
#define QUADCOPTIC_CONTROLINPUT_H

#include <iostream>

struct ControlInput
{
    enum Input
    {
        THRUST,
        ROLL,
        PITCH,
        YAW
    };

    ControlInput()
            : thrust(0.),
              roll(0.),
              pitch(0.),
              yaw(0.)
    {
    }

    explicit ControlInput(double u[4])
            : thrust(u[0]),
              roll(u[1]),
              pitch(u[2]),
              yaw(u[3])
    {
    }

    ControlInput(double throttle, double roll, double pitch, double yaw)
            : thrust(throttle),
              roll(roll),
              pitch(pitch),
              yaw(yaw)
    {
    }

    double &operator[](int index)
    {
        switch (index)
        {
            case 0:
                return thrust;
            case 1:
                return roll;
            case 2:
                return pitch;
            case 3:
                return yaw;
            default:
                throw std::out_of_range("Control input index out of range");
        }
    }

    friend std::ostream &operator<<(std::ostream &out, const ControlInput &u)
    {
        out << "[" << u.thrust << ", " << u.roll << ", " << u.pitch << ", " << u.yaw << "]";
        return out;
    }

    double thrust;
    double roll;
    double pitch;
    double yaw;
};

#endif //QUADCOPTIC_CONTROLINPUT_H

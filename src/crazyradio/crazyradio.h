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

#ifndef CRAZYRADIO_H
#define CRAZYRADIO_H

#include "abstractradio.h"

class CrazyRadio : public AbstractRadio
{

public:

    CrazyRadio(int id, std::string txId);

    virtual ~CrazyRadio();

    void setControls(double throttle, double roll, double pitch, double yaw);

    void toggleSuspension();

    void suspend(bool suspend);

    void emergencyStop(bool _emergency = true);

    // not implemented /////////////////////////////////////////////////////////

    void toggleSender();

    void turnSenderOn();

    void turnSenderOff();

    void setBindSignal();

    void setArmSignal();

    void setDisarmSignal();

    void toggleGear();

    void toggleAux1();
};

#endif /* CRAZYRADIO_H */

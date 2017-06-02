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

#ifndef ICARUS_CRAZYRADIOMODULE_H
#define ICARUS_CRAZYRADIOMODULE_H

#include "abstracttxmodule.h"

class CrazyRadioModule : public AbstractTxModule
{

public:

    CrazyRadioModule(int id, std::string sender, std::string txId);

    virtual ~CrazyRadioModule();

    void setControls(double throttle, double roll, double pitch, double yaw);

    void toggleSuspension();

    void suspend(bool suspend);

    void emergencyStop(bool emergency = true);

    void setSuspensionSignal();

    // not implemented /////////////////////////////////////////////////////////

    void toggleSender();

    void turnSenderOn();

    void turnSenderOff();

    void setBindSignal();

    void toggleGear();

    void toggleAux1();
};

#endif //ICARUS_CRAZYRADIOMODULE_H

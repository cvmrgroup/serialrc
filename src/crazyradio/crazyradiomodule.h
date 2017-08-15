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

#include <radio/abstracttxmodule.h>

class CrazyRadioModule : public AbstractTxModule
{

public:

    CrazyRadioModule(int id, std::string sender, std::string txId);

    void setSuspensionSignal();

    // /////////////////////////////////////////////////////////////////////////

    void setControls(double throttle, double roll, double pitch, double yaw) override;

    void toggleSuspension() override;

    void suspend(bool suspend) override;

    void emergencyStop() override;

    // not implemented /////////////////////////////////////////////////////////

    void toggleSender() override;

    void turnSenderOn() override;

    void turnSenderOff() override;

    void setBindSignal() override;

    void toggleGear() override;

    void setGear(bool state) override;

    void toggleAux1() override;
};

#endif //ICARUS_CRAZYRADIOMODULE_H

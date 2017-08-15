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

#ifndef DSMXMODULE_H
#define DSMXMODULE_H

#include <algorithm>

#include <config/radioconfig.h>

#include <radio/abstracttxmodule.h>

class DSMXModule : public AbstractTxModule
{

public:

    DSMXModule(int copterId, RadioConfig &config);

    /**
     * Get the radio configuration.
     * @return The radio configuration.
     */
    RadioConfig getRadioConfig();

    void setArmSignal();

    void setDisarmSignal();

    // /////////////////////////////////////////////////////////////////////////

    void toggleSender() override;

    void turnSenderOn() override;

    void turnSenderOff() override;

    void suspend(bool suspended) override;

    void toggleSuspension() override;

    void setBindSignal() override;

    void setControls(double throttle, double roll, double pitch, double yaw) override;

    void toggleGear() override;

    void setGear(bool state) override;

    void toggleAux1() override;

    void emergencyStop() override;

private:

    RadioConfig config;
};

#endif /* DSMXMODULE_H */

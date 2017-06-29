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

    virtual ~DSMXModule();

    /**
     * Get the radio configuration.
     * @return The radio configuration.
     */
    RadioConfig getRadioConfig();

    void toggleSender();

    void turnSenderOn();

    void turnSenderOff();

    void suspend(bool suspended);

    void toggleSuspension();

    void setBindSignal();

    void setControls(double throttle, double roll, double pitch, double yaw);

    void toggleGear();

    void toggleAux1();

    void setArmSignal();

    void setDisarmSignal();

    void emergencyStop(bool emergency = true);

private:

    RadioConfig config;
};

#endif /* DSMXMODULE_H */

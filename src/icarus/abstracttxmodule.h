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
 *       @date:   16.02.2015
 *****************************************************/

#ifndef ABSTRACTTXMODULE_H
#define ABSTRACTTXMODULE_H

#include <string>
#include <vector>

#include "icarus/controlinput.h"

class AbstractTxModule
{

public:

    // Make channels addressable by default names
    enum ChannelNames
    {
        Throttle,
        Aileron,
        Elevation,
        Rudder,
        Gear,
        Aux1
    };

    /**
     * Construct.
     * @param copterId The copter id.
     * @param txName The sender name.
     * @param moduleId The transmitter id.
     */
    AbstractTxModule(int copterId, std::string txName, int moduleId);

    /**
     * Destroy.
     */
    virtual ~AbstractTxModule() = default;

    // /////////////////////////////////////////////////////////////////////////

    /**
     * Method to toggle a copters suspend state by overwriting
     * throttle with inital throttle value.
     */
    virtual void toggleSuspension() = 0;

    /**
     * Method to suspend a copter which is realized by overwriting
     * throttle with inital throttle value.
     * @param suspended The state of suspension.
     */
    virtual void suspend(bool suspend) = 0;

    /**
     * Method to set commands by flight control. Values normalized
     * between -1.0 and 1.0
     * @param ch1 Normalized throttle value.
     * @param ch2 Normalized roll value.
     * @param ch3 Normalized pitch value.
     * @param ch4 Normalized yaw value.
     */
    virtual void setControls(ControlInput u) = 0;

    // DSMX ////////////////////////////////////////////////////////////////////

    virtual bool isEnabled();

    virtual bool isBinding();

    virtual void toggleSender();

    virtual void setBindSignal();

    virtual void toggleGear();

    virtual void setGear(bool state);

    virtual void toggleAux1();

    // /////////////////////////////////////////////////////////////////////////

    virtual void reset();

    // /////////////////////////////////////////////////////////////////////////

    /**
     * Get the id of the copter this tx module is bound to.
     * @return The copter id bound to this module.
     */
    int getCopterId();

    /**
     * Get the sender name (as defined in config).
     * @return The sender name.
     */
    std::string getTxName();

    /**
     * Get the transmitter module id.
     * @return The transmitter module id.
     */
    int getModuleId();

    /**
     * Get throttle value.
     * @return The throttle value.
     */
    std::vector<double> getSignal();

    /**
     * Get the number of radio channels.
     * @return The number of channels.
     */
    int getNumberOfChannels();

    /**
     * Method to retrieve the current suspension mode.
     * @return True if suspended, otherwise false.
     */
    bool isSuspended();

protected:

    int copterId;
    std::string txName;
    int moduleId;

    std::vector<double> signal;

    int nChannels;
    bool suspended;
};

#endif /* ABSTRACTTXMODULE_H */

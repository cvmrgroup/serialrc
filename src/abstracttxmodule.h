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

class AbstractTxModule
{
public:

    /**
     * Number of channels.
     */
    static const int N_CHANNELS = 6;

    /**
     * Common channel names.
     */
    enum Channel
    {
        Throttle = 0,
        Aileron = 1,
        Elevation = 2,
        Rudder = 3,
        Gear = 4,
        Aux1 = 5
    };

    /**
     * Construct.
     * @param id The radio id.
     * @param txName The sender name.
     * @param moduleId The transmitter name.
     */
    AbstractTxModule(int copterId, std::string txName, std::string moduleId);

    /**
     * Destroy.
     */
    virtual ~AbstractTxModule();

    /**
     * Method to toggle transmitter state by its id.
     * @param id The Id.
     */
    virtual void toggleSender() = 0;

    /**
     * @brief set boolean to turn on relais.
     * @param id the transmitter id.
     */
    virtual void turnSenderOn() = 0;

    /**
     * @brief set boolean to turn off relais.
     * @param id the transmitter id.
     */
    virtual void turnSenderOff() = 0;

    /**
     * Method to set the binding header and switch on the relay the
     * transmitter is attached to.
     */
    virtual void setBindSignal() = 0;

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
    virtual void setControls(double throttle, double roll, double pitch,
                             double yaw) = 0;

    /**
     * Toggle Gear (Channel 5).
     */
    virtual void toggleGear() = 0;

    /**
     * Toggle Aux (Channel 6).
     */
    virtual void toggleAux1() = 0;

    /**
     * Send Emergency command to copter.
     * @param emergency Bool (todo what for?).
     */
    virtual void emergencyStop(bool emergency = true) = 0;

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
    std::string getModuleId();

    /**
     * Method to retrieve the currently active transmitters.
     * @return Array to be filled with states.
     */
    bool isEnabled();

    /**
     * Check if transmitter is in bind mode.
     * @return wether the transmitter is in bind mode or not.
     */
    bool isBinding();

    /**
     * Method to retrieve the current suspension mode.
     * @return Array to be filled with states.
     */
    bool isSuspended();

    /**
     * Get throttle value.
     * @return The throttle value.
     */
    double getThrottle();

    /**
     * Get roll value.
     * @return The roll value.
     */
    double getRoll();

    /**
     * Get pitch value.
     * @return The pitch value.
     */
    double getPitch();

    /**
     * Get yaw value.
     * @return The yaw value.
     */
    double getYaw();

    /**
     * Get gear value (Channel 5).
     * @return The gear value.
     */
    double getGear();

    /**
     * Get aux 1 (Channel 6) value.
     * @return The aux 1 value.
     */
    double getAux1();

    /**
     * Check if radio is in emergency mode.
     * @return True if in emergency mode, otherwise false.
     */
    bool isEmergency();

protected:

    int copterId;

    std::string txName;

    std::string moduleId;

    double signal[N_CHANNELS];

    bool binding;
    bool enabled;
    bool suspended;
    bool emergency;
};

#endif /* ABSTRACTTXMODULE_H */

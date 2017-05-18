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

#ifndef ABSTRACTRADIO_H
#define ABSTRACTRADIO_H

#include <string>

class AbstractRadio
{
public:

    static const int N_CHANNELS = 6;

    enum Channel
    {
        Throttle = 0,
        Aileron = 1,
        Elevation = 2,
        Rudder = 3,
        Gear = 4,
        Aux1 = 5
    };

    AbstractRadio(int id, std::string sender, std::string txId);

    virtual ~AbstractRadio();

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
     * Sets the suspension signal (lower throttle lever position).
     */
    virtual void setSuspensionSignal() = 0;

    /**
     *  This method sends low throttle and full yaw to arm multiwii copter.
     *  @param id The transmitter id.
     */
    virtual void setArmSignal() = 0;

    /**
     *  This method sends low throttle and low yaw to disarm multiwii copter.
     *  @param id The transmitter id.
     */
    virtual void setDisarmSignal() = 0;

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

    virtual void toggleGear() = 0;

    virtual void toggleAux1() = 0;

    /**
     * Send Emergency command to copter
     */
    virtual void emergencyStop(bool _emergency = true) = 0;

    /**
     *  get id
     *  @return the id
     */
    int getId();

    /**
     * Get the sender name (as defined in config)
     * @return The sender name
     */
    std::string getSender();

    /**
     *  get current transmitter id.
     *  @return the transmitter id
     */
    std::string getTxId();

    /**
     * Method to retrieve the currently active transmitters.
     * @return Array to be filled with states.
     */
    bool isEnabled();

    /**
     *  check if transmitter is in bind mode.
     *
     *  @return wether the transmitter is in bind mode or not.
     */
    bool isBinding();

    /**
     * Method to retrieve the current suspension mode.
     * @return Array to be filled with states.
     */
    bool isSuspended();

    double getThrottle();

    double getRoll();

    double getPitch();

    double getYaw();

    double getGear();

    double getAux1();

    bool isEmergency();

protected:

    int id;

    std::string sender;

    std::string txId;

    double signal[N_CHANNELS];

    bool binding;
    bool enabled;
    bool suspended;
    bool emergency;
};

#endif /* ABSTRACTRADIO_H */

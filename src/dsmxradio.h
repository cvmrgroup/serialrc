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

#ifndef DSMXRADIO_H
#define DSMXRADIO_H

#include "abstractradio.h"

/**
 *  @todo invert flag for roll and pitch of super x copter
 *  nano   +roll / +yaw
 *  superx -roll / -yaw
 */
class DSMXRadio: public AbstractRadio
{
    
public:
    
    DSMXRadio(unsigned int id, unsigned int txId, std::string identifier);
    virtual ~DSMXRadio();
    
    void toggleSender();
    
    void turnSenderOn();
    
    void turnSenderOff();
    
    void suspend(bool suspended);
    
    void toggleSuspension();
    
    void setBindSignal();
    
    void setControls(double throttle, double roll, double pitch, double yaw);
    
    void toggleCh5();
    
    void toggleCh6();
    
    void setArmSignal();
    
    void setDisarmSignal();
};

#endif /* DSMXRADIO_H */

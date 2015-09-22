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

#ifndef CRAZYRADIOTHREAD_H
#define CRAZYRADIOTHREAD_H

#include <iostream>

#include <QThread>

#include <opencv2/core.hpp>

#include <cflie/CCrazyflie.h>

#include "abstractradio.h"

class CrazyRadioThread: public QThread
{
    Q_OBJECT
    
public:
    
    CrazyRadioThread();
    virtual ~CrazyRadioThread();
    
    bool open();
    void run();
    void stop();
    
    void setRadio(AbstractRadio* radio);
    
private:
    
    CCrazyRadio* radio;
    CCrazyflie* copter;
    
    AbstractRadio* radiotransmitter;
    
    volatile bool doRun;
};

#endif /* CRAZYRADIOTHREAD_H */

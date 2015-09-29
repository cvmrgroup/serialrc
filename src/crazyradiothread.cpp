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
 *       @date:   24.06.2015
 *****************************************************/

#include "crazyradiothread.h"

using namespace std;

CrazyRadioThread::CrazyRadioThread()
{
    radio = 0;
    copter = 0;
    
    radiotransmitter = 0;
    
    doRun = false;
}

CrazyRadioThread::~CrazyRadioThread()
{
    if(radio != 0)
    {
        delete radio;
    }
    
    if(copter != 0)
    {
        // todo delete copter;
    }
}

bool CrazyRadioThread::open()
{
    radio = new CCrazyRadio("radio://0/80/250K");
    
    if(radio->startRadio())
    {
        copter = new CCrazyflie(radio);
        copter->setSendSetpoints(true);
        copter->setThrust(0);
        
        cout << "start radio" << endl;
        cout << copter->isInitialized() << endl;
        
        doRun = true;
        return true;
    }
    
    return false;
}

void CrazyRadioThread::run()
{
    cout << "CrazyRadio: " << (radio!=0) << " " << (copter!=0) << endl;

    if(radio == 0 || copter == 0)
    {
        // not initialized
        return;
    }
    
    while(doRun)
    {
//        if(!copter->isInitialized())
//        {
//            cout << "copter not initialized" << endl;
//            msleep(10);
//            continue;
//        }
        
        if(copter->cycle())
        {
            //cout << copter->copterInRange() << " " << copter->sendsSetpoints() << " " << copter->batteryLevel() << " " << copter->thrust() << " " << copter->roll() << " " << copter->pitch() << " " << copter->yaw() << endl;
            
            if(radiotransmitter != 0)
            {
                int throttle = (int) radiotransmitter->getThrottle();
                float roll = (float) radiotransmitter->getRoll();
                float pitch = (float) radiotransmitter->getPitch();
                float yaw = (float) radiotransmitter->getYaw();
                
                copter->setThrust(throttle);
                copter->setRoll(roll);
                copter->setPitch(pitch);
                copter->setYaw(yaw);
            }
        }
        
        // todo msleep(10);
    }
}

void CrazyRadioThread::stop()
{
    doRun = false;
}

void CrazyRadioThread::setRadio(AbstractRadio* _radio)
{
    radiotransmitter = _radio;
}
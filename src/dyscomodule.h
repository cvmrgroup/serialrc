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
/** @copyright    Hochschule RheinMain,
 *                University of Applied Sciences
 *     @author    Marc Lieser
 *    @version    1.0
 *       @date    27.06.2017
*****************************************************/

#ifndef ICARUS_DYSCOMODULE_H
#define ICARUS_DYSCOMODULE_H

#include <opencv2/core.hpp>

#include <boost/signals2.hpp>

#include <radio/abstracttxmodule.h>

class DyscoModule : public AbstractTxModule
{
public:

    DyscoModule(int copterId, std::string txName, std::string moduleId);

    virtual ~DyscoModule();

    void setControls(double throttle, double roll, double pitch, double yaw);

    void toggleGear();

    void toggleAux1();

    // not implemented /////////////////////////////////////////////////////////

    void toggleSuspension();

    void suspend(bool suspend);

    void emergencyStop(bool emergency = true);

    void setSuspensionSignal();

    void toggleSender();

    void turnSenderOn();

    void turnSenderOff();

    void setBindSignal();

    // /////////////////////////////////////////////////////////////////////////

    boost::signals2::signal<void(int, cv::Vec4d)> onControlCommand;
    boost::signals2::signal<void(int)> onResetCommand;
    boost::signals2::signal<void(int)> onHorizonCommand;
};

#endif //ICARUS_DYSCOMODULE_H

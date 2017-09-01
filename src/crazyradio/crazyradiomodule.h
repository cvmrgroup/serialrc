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

    CrazyRadioModule(int copterId, std::string sender, int txId, std::string radioUri);

    // AbstractTxModule ////////////////////////////////////////////////////////

    void setControls(ControlInput u) override;

    void toggleSuspension() override;

    void suspend(bool suspend) override;

    // /////////////////////////////////////////////////////////////////////////

    std::string getRadioUri();

    void setSuspensionSignal();

private:

    std::string radioUri;
};

#endif //ICARUS_CRAZYRADIOMODULE_H

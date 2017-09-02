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

#include <config/txmoduleconfig.h>

#include <radio/abstracttxmodule.h>

class DSMXModule : public AbstractTxModule
{

public:

    DSMXModule(int copterId, TxModuleConfig &config);

    // AbstractTxModule ////////////////////////////////////////////////////////

    void toggleSuspension() override;

    void suspend(bool suspended) override;

    void setControls(ControlInput u) override;

    void toggleSender() override;

    void setBindSignal() override;

    void toggleGear() override;

    void setGear(bool state) override;

    void toggleAux1() override;

    bool isEnabled() override;

    bool isBinding() override;

    // /////////////////////////////////////////////////////////////////////////

    TxModuleConfig getConfig();

    void setArmSignal();

    void setDisarmSignal();

private:

    TxModuleConfig config;

    bool enabled;
    bool binding;
};

#endif /* DSMXMODULE_H */

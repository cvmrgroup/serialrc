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
 *       @date    20.11.2016
 *****************************************************/

#ifndef ICARUS_DYSCOTX_H
#define ICARUS_DYSCOTX_H

#include <unordered_map>

#include <boost/lexical_cast.hpp>

#include <radio/i_transmitter.h>

#include <radio/commands/controlcommand.h>
#include <radio/commands/switchgearcommand.h>
#include <radio/commands/switchaux1command.h>

class DyscoTx : public ITransmitter
{

public:

    DyscoTx(std::string name);

    virtual ~DyscoTx();

    virtual void open();

    virtual void close();

    // /////////////////////////////////////////////////////////////////////////

    virtual std::string getName();

    virtual bool isOpen();

    virtual bool hasCapacity();

    virtual void addTxModule(AbstractTxModule *module);

private:

    std::string name;

    std::unordered_map<int, AbstractTxModule *> modules;
};

#endif //ICARUS_DYSCOTX_H

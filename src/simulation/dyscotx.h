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

#include <interfaces/i_transmitter.h>

#include <radio/radiocommands.h>

#include <radio/abstracttxmodule.h>

class DyscoTx : public ITransmitter
{

public:

    explicit DyscoTx(std::string name);

    ~DyscoTx() override = default;

    void open() override;

    void close() override;

    // /////////////////////////////////////////////////////////////////////////

    std::string getName() override;

    bool isOpen() override;

    bool hasCapacity() override;

    void addTxModule(AbstractTxModule *module) override;

private:

    std::string name;

    std::unordered_map<int, AbstractTxModule *> modules;
};

#endif //ICARUS_DYSCOTX_H

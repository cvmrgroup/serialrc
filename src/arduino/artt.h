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

#ifndef ARTT_H
#define ARTT_H

#include <vector>

#include "arxx.h"
#include "dsmxconstants.h"
#include "attdefinitions.h"

/**
 * Software-side class for communication with the software running on the
 * Arduino triple transmission built-in Arduino board.
 */
class ArTT : public ArXX
{

public:

    /**
     * Constructor.
     * @param name Device name.
     * @param serial Device's serial number.
     * @param ioService The boost io service.
     */
    ArTT(std::string name,
         std::string serial,
         boost::shared_ptr<boost::asio::io_service> ioService);

    /**
     * Overwrite addTxModule function for getting the transmitter ids
     * @param the AbstractRadio to add to this transmitter
     */
    void addTxModule(AbstractTxModule *txModule) override;

    /**
     * Clean up.
     */
    void stop() override;

private:

    /**
     * Function called when a message delimited by ';' (0x3b) is received.
     * @param frame The data frame.
     * @param length Length of the array.
     */
    void onData(std::string frame) override;

    /**
     * Write data to the serial port.
     * @param id The transmitter id.
     */
    void writeData(int id = 0);

    /// Radio configs.
    std::unordered_map<int, RadioConfig> configs;
    std::vector<int> moduleIds;

    /// The tx id the last frame was sent to.
    int lastModuleId;

    /// We need to know the first serial message arrives to ignore it.
    bool first;

    /// Remember whos turn is next.
    unsigned int token;
};

#endif /* ARTT_H */

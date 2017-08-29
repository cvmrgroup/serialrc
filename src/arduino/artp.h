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
 *       @date:   03.06.2014
 *****************************************************/

#ifndef ARTP_H
#define ARTP_H

#include "arxx.h"
#include "atpdefinitions.h"
#include "trainerportconstants.h"

/**
 * Software-side class for communication with the software running on the
 * tx3 built-in arduino.
 * This class implements a QThread and runs an infinite loop receiving a
 * send signal from the arduino and answering with a new copter command
 * if a new command was set.
 */
class ArTP : public ArXX
{

public:

    /**
     * Constructor.
     * @param name Device name.
     * @param serial Device's serial number.
     * @param ioService The boost io service.
     */
    ArTP(std::string name,
         std::string serial,
         boost::shared_ptr<boost::asio::io_service> ioService);

    /**
     * Overwrite addTxModule function for getting the transmitter ids
     * @param the AbstractRadio to add to this transmitter
     */
    void addTxModule(AbstractTxModule *txModule) override;

private:

    /**
     * Receive data from the serial port.
     * @param frame The data frame.
     * @param length The lenght of the data frame.
     */
    void onData(std::string frame) override;

    /**
     * Send data to the serial port.
     */
    void writeData();

    /// Flag for the first incoming message
    bool first;

    /// The data frame
    unsigned char frame[ATP_FRAME_LENGTH];
};

#endif /* ARTP_H */

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

#ifndef ARXX_H
#define ARXX_H

#include <string>
#include <fstream>
#include <unordered_map>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio/deadline_timer.hpp>

#include <radio/i_transmitter.h>

#include "dsmxmodule.h"
#include "serialhelper.h"

#include "attdefinitions.h"

/**
 * @brief Software-side super class for the communication with the software
 * running on the Arduinos used for Arduino Triple Transmission and Arduino
 * Trainer Port.
 */
class ArXX : public ITransmitter
{

public:

    /**
     * Constructor.
     * @param name The device name.
     * @param maxNumberOfRadios The number of radios.
     * @param portName The port name of the Arduino is connected to.
     * @param ioService The boost io service.
     */
    ArXX(std::string name,
         int maxNumberOfRadios,
         std::string portName,
         boost::shared_ptr<boost::asio::io_service> ioService);

    ArXX() = delete;

    ~ArXX() override = default;

    ArXX(ArXX const &other) = delete;

    void operator=(ArXX const &other) = delete;

private:

    void invokeReading(boost::system::error_code errorCode,
                       boost::shared_ptr<boost::asio::deadline_timer> timer);

    void readData();

    void onDataRead(boost::system::error_code errorCode,
                    size_t bytes);

    void onDataWritten(boost::system::error_code errorCode,
                       boost::shared_ptr<boost::asio::streambuf> buf,
                       size_t bytes);

public:

    // ITransmitter ////////////////////////////////////////////////////////////

    void open() override;

    void close() override;

    std::string getName() override;

    bool isOpen() override;

    bool hasCapacity() override;

    void addTxModule(AbstractTxModule *txModule) override;

    // /////////////////////////////////////////////////////////////////////////

    int getNumberOfRadios();

    int getMaxNumberOfRadios();

    void suspendAll();

    void getSuspendedTxs(std::unordered_map<int, bool> &suspended);

    void getEnabledStates(std::unordered_map<int, bool> &enableds);

protected:

    /**
     * Write the given frame to the serial port.
     * @param frame The frame to write to the serial port.
     * @param The length of the frame.
     */
    void writeFrame(const char *frame, size_t length);

    /**
     * Called when a data frame was received on the underlying serial port.
     * @param frame The received frame as char array.
     * @param length The length of the received frame.
     */
    virtual void onData(std::string frame) = 0;

protected:

    /// The port name the Arduino is connected to.
    std::string portName;

private:

    /// The name of the transmitter.
    std::string transmitterName;

    /// Maximum number of connectable transmitter modules.
    int maxNumberOfModules;

    /// The io service to handle incoming and outgoing serial data.
    boost::shared_ptr<boost::asio::io_service> ioService;

    /// The serial port.
    boost::shared_ptr<boost::asio::serial_port> serialPort;

    /// The response stream buffer.
    boost::asio::streambuf response;

protected:

    /// A map with all transmitter modules connected to this transmitter.
    std::unordered_map<int, AbstractTxModule *> modules;
};

#endif //ARXX_H

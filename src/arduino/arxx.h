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

#include <QObject>
#include <QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "dsmxmodule.h"
#include "serialhelper.h"
#include "i_transmitter.h"
#include "radioexception.h"

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
     * @brief Constructor.
     */
    ArXX(const std::string name, int maxNumberOfRadios,
         std::string serialNumber,
         boost::shared_ptr<boost::asio::io_service> io_service);

    virtual ~ArXX();

private:

    std::string findDeviceName(std::string &description);

    void invokeReading(boost::system::error_code ec,
                       boost::shared_ptr<boost::asio::deadline_timer> timer);

    void readData();

    void onDataRead(boost::system::error_code ec,
                    boost::shared_ptr<boost::asio::streambuf> buf,
                    size_t bytes);

    void onDataWritten(boost::system::error_code ec,
                       boost::shared_ptr<boost::asio::streambuf> buf,
                       size_t bytes);

public:

    void open();

    void close();

    int getMaxNumberOfRadios();

    int getNumberOfRadios();

    std::string getSerialNumber();

    bool hasCapacity();

    void addTxModule(AbstractTxModule *txModule);

    void suspendAll();

    void getSuspendedTxs(std::unordered_map<int, bool> &suspended);

    void getEnabledStates(std::unordered_map<int, bool> &enableds);

    std::string getName();

    bool isOpen();

protected:

    /**
     * Called when a data frame was received on the underlying serial port.
     * @param frame The received frame as char array.
     * @param length The length of the received frame.
     */
    virtual void onData(const char *frame, size_t length) = 0;

    /**
     * Write the given frame to the serial port.
     * @param frame The frame to write to the serial port.
     * @param The length of the frame.
     */
    void writeFrame(const char *frame, size_t length);

    /// A map with all transmitter modules connected to this transmitter.
    std::unordered_map<int, AbstractTxModule *> modules;

private:

    /// Maximum number of connectable transmitter modules.
    int maxNumberOfModules;

    /// The name of the transmitter.
    std::string transmitterName;

    /// The serial number of this device.
    std::string serialNumber;

    /// The io service to handle incoming and outgoing serial data.
    boost::shared_ptr<boost::asio::io_service> ioService;

    /// The serial port.
    boost::shared_ptr<boost::asio::serial_port> serialPort;
};

#endif //ARXX_H

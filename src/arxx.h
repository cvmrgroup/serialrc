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

#include <iostream>
#include <string>

#include <opencv2/core.hpp>

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>

#include <boost/log/trivial.hpp>

#include "serialhelper.h"
#include "dsmxradio.h"

/// signal to wait for before sending a new command
#define XON 0x11

/// baud rate used for communication
#define BAUD_RATE B115200

/**
 * @brief Software-side class for communication with the software running on the
 * arduino triple transmission built-in arduino board.
 */
class ArXX
{
    
public:
    
    /**
     * @brief Constructor.
     */
    ArXX(int maxNumberOfRadios, std::string serialNumber);
    
    /**
     * @brief Destructor.
     */
    virtual ~ArXX();
    
    /**
     *  @brief initialize the serial port.
     */
    bool open(boost::asio::io_service* io);
    
    /**
     * @brief finalize serial port.
     */
    void close();
    
    int getMaxNumberOfRadios();
    int getNumberOfRadios();
    bool hasCapacity();
    void addRadio(AbstractRadio* radio);
    void suspendAll();
    void getSuspendedTxs(std::vector<bool>& suspended);
    
    /**
     *  @brief read data from serial port.
     */
    virtual void readData(boost::system::error_code ec, size_t bytes) = 0;
    
    void handle_write(boost::system::error_code ec, size_t bytes);
    
protected:
    
    /**
     *  write data to serial port.
     */
    virtual void writeData(unsigned int id = 0) = 0;
    
    boost::asio::serial_port* serialPort;
    
    boost::asio::streambuf buf;
    
    std::string serialNumber;
    
    /**
     *  max number of radios.
     */
    int maxNumberOfRadios;
    
    /**
     *  the radios
     */
    std::vector<AbstractRadio*> radios;
};

#endif /* ARXX_H */

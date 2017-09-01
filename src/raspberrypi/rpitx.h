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
 *       @date:   27.04.2017
 *****************************************************/

#ifndef RPITX_H
#define RPITX_H

#include <atomic>

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/log/trivial.hpp>

#include <wiringPi.h>

#include <timing/deadlinetimer.h>

#include "dsmxmodule.h"
#include "dsmxconstants.h"
#include "abstracttxmodule.h"
#include "i_transmitter.h"
#include "serialhelper.h"
#include "serial.h"

class RpiTX : public ITransmitter
{

public:

    RpiTX(const std::string name,
          const std::string serialPortName,
          boost::shared_ptr<boost::asio::io_service> io_service);

    void open();

    void close();

    std::string getName();

    bool isOpen();

    bool hasCapacity();

    void addTxModule(AbstractTxModule *txModule);

    void update(const boost::system::error_code &ec);

private:

    /// the name of the transmitter
    std::string name;
    std::string serialPortName;
    int serial;
    bool initialized;

    DSMXModule *module;

    boost::shared_ptr<boost::asio::io_service> io_service;
    DeadlineTimer *timer;

    static const int DSM_FRAME_LENGTH = 14;
    static const int DSM_BAUD_RATE = 125000;

    static const int POWER_PIN = 0;

    /// channel ids
    static const int header_1 = 0;
    static const int header_2 = 1;
    static const int channel_1_hi = 2;
    static const int channel_1_lo = 3;
    static const int channel_2_hi = 4;
    static const int channel_2_lo = 5;
    static const int channel_3_hi = 6;
    static const int channel_3_lo = 7;
    static const int channel_4_hi = 8;
    static const int channel_4_lo = 9;
    static const int channel_5_hi = 10;
    static const int channel_5_lo = 11;
    static const int channel_6_hi = 12;
    static const int channel_6_lo = 13;
};

#endif /* RPITX_H */

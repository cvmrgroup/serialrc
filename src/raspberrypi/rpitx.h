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

//#include <wiringPi.h>

#include <atomic>

#include <boost/thread.hpp>
#include <boost/log/trivial.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/log/trivial.hpp>

#include "i_transmitter.h"

class RpiTX : public ITransmitter
{

public:

    RpiTX(const std::string name, boost::shared_ptr<boost::asio::io_service> io_service);

    void open();

    void close();

    std::string getName();
	
    bool isOpen();

    bool hasCapacity();

    void addRadio(AbstractRadio *radio);

    void start();
    
    void run();

    void stop();

    void join();

    bool isRunning();
    
private:

    /// the name of the transmitter
    std::string name;

    bool initialized;

    std::atomic<bool> running;
    boost::shared_ptr<boost::asio::io_service> io_service;
    boost::shared_ptr<boost::thread> thread;
    //boost::shared_ptr<boost::asio::io_service::work> work;
};

#endif /* RPITX_H */

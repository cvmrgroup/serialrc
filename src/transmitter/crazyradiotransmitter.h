//
// Created by lasse on 29.09.15.
//

#ifndef ICARUS_CRAZYRADIOTRANSMITTER_H
#define ICARUS_CRAZYRADIOTRANSMITTER_H

#include <atomic>
#include <unordered_map>

#include <CCrazyflie.h>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/log/trivial.hpp>
#include <boost/thread/latch.hpp>

#include <opencv2/core.hpp>

#include <clock.h>

#include "i_transmitter.h"
#include "radioexception.h"

class CrazyRadioTransmitter : public ITransmitter
{
public:

    CrazyRadioTransmitter(const std::string name, boost::shared_ptr<boost::asio::io_service> io_service);

private:

    void run();

    void initialize();

    void update();

    void cleanup();

    void publishCopterData();

public:

    void open();

    void close();

    // ###########################################

    std::string getName();

    bool isOpen();

    bool hasCapacity();

    void addRadio(AbstractRadio *radio);

private:

    /// the id of the current radio
    std::string currentRadio;

    /// flag if the CrazyRadioTransmitter is running
    std::atomic<bool> running;

    /// the name of the i_transmitter
    std::string name;

    /// the map with radios
    std::unordered_map<std::string, AbstractRadio *> radios;

    /// the crazyflie copter
    boost::shared_ptr<CCrazyflie> copter;
    /// the crazy radio
    boost::shared_ptr<CCrazyRadio> radio;

    /// the boost thread for the update
    boost::shared_ptr<boost::thread> thread;

    /// the underlying used io_service
    boost::shared_ptr<boost::asio::io_service> io_service;
};

#endif //ICARUS_CRAZYRADIOTRANSMITTER_H

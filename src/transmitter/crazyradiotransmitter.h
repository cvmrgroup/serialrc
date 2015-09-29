//
// Created by lasse on 29.09.15.
//

#ifndef ICARUS_CRAZYRADIOTRANSMITTER_H
#define ICARUS_CRAZYRADIOTRANSMITTER_H

#include <atomic>
#include <unordered_map>

#include <cflie/CCrazyflie.h>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include "i_transmitter.h"
#include "radioexception.h"

class CrazyRadioTransmitter : public ITransmitter
{
public:

    CrazyRadioTransmitter(const std::string name, boost::shared_ptr<boost::asio::io_service> io_service);

private:

    void update(const boost::system::error_code &ec);

    void invokeTimer();

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
    int currentRadio;

    /// flag if the CrazyRadioTransmitter is running
    std::atomic<bool> running;

    /// the name of the i_transmitter
    std::string name;

    /// the map with radios
    std::unordered_map<int, AbstractRadio *> radios;

    /// the crazyflie copter
    boost::shared_ptr<CCrazyflie> copter;
    /// the crazy radio
    boost::shared_ptr<CCrazyRadio> radio;

    /// the underlying used io_service
    boost::shared_ptr<boost::asio::io_service> io_service;
    /// the boost timer for updating the crazyflie copter
    boost::shared_ptr<boost::asio::deadline_timer> timer;

};


#endif //ICARUS_CRAZYRADIOTRANSMITTER_H

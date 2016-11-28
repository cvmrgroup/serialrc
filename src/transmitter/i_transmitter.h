//
// Created by lasse on 29.09.15.
//

#ifndef ICARUS_I_TRANSMITTER_H
#define ICARUS_I_TRANSMITTER_H

#include <string>

#include <boost/signals2.hpp>

#include <radio/telemetry.h>

#include "radio/abstractradio.h"

class ITransmitter
{

public:

    virtual ~ITransmitter()
    {
    };

    /**
     * open the transmitter
     */
    virtual void open() = 0;

    /**
     * close the transmitter
     */
    virtual void close() = 0;

    // /////////////////////////////////////////////////////////////////////////

    /**
     * gets the name of the Transmitter
     * @return the name of the transmitter
     */
    virtual std::string getName() = 0;

    /**
     * checks if the ITransmitter is open
     * @return true if the ITransmitter is open, otherwise false
     */
    virtual bool isOpen() = 0;

    /**
     * checks if the radio has Radio capacity left
     * @return true if capacity is left, otherwise false
     */
    virtual bool hasCapacity() = 0;

    /**
     * add the given Radio to this ITransmitter
     * @param radio the radio to add to this ITransmitter
     */
    virtual void addRadio(AbstractRadio *radio) = 0;

    // /////////////////////////////////////////////////////////////////////////

    /// the boost signal to publish telemetry data
    boost::signals2::signal<void(Telemetry data)> onTelemetry;
};

#endif //ICARUS_I_TRANSMITTER_H

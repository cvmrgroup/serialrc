//
// Created by lasse on 29.09.15.
//

#ifndef ICARUS_I_TRANSMITTER_H
#define ICARUS_I_TRANSMITTER_H

#include <string>

#include <boost/signals2.hpp>

#include <radio/telemetry.h>

#include "abstracttxmodule.h"

class ITransmitter
{

public:

    virtual ~ITransmitter(){};

    /**
     * Open the transmitter.
     */
    virtual void open() = 0;

    /**
     * Close the transmitter.
     */
    virtual void close() = 0;

    // /////////////////////////////////////////////////////////////////////////

    /**
     * Gets the name of the transmitter.
     * @return The name of the transmitter.
     */
    virtual std::string getName() = 0;

    /**
     * Checks if the ITransmitter is open.
     * @return True if the ITransmitter is open, otherwise false.
     */
    virtual bool isOpen() = 0;

    /**
     * Checks if the radio has Radio capacity left.
     * @return true if capacity is left, otherwise false.
     */
    virtual bool hasCapacity() = 0;

    /**
     * Add the given transmitter module to this transmitter
     * @param module The transmitter module to add to this transmitter
     */
    virtual void addTxModule(AbstractTxModule *module) = 0;

    // /////////////////////////////////////////////////////////////////////////

    /// The boost signal to publish telemetry data
    boost::signals2::signal<void(Telemetry data)> onTelemetry;
};

#endif //ICARUS_I_TRANSMITTER_H

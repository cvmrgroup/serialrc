//
// Created by lasse on 28.09.15.
//

#include "ioserviceradio.h"

IOServiceRadio::IOServiceRadio(std::vector<RadioConfig> configs,
                               boost::shared_ptr<boost::asio::io_service> io_service)
{
    this->started = false;
    this->configs = configs;
    this->io_service = io_service;
}

IOServiceRadio::~IOServiceRadio()
{
    for (auto entry: this->radios)
    {
        AbstractRadio *radio = entry.second;
        delete radio;
    }

    this->radios.clear();

    for (auto entry : this->transmitters)
    {
        ITransmitter *transmitter = entry.second;
        // check if the ITransmitter is open
        if (transmitter->isOpen())
        {
            //close the ITransmitter
            transmitter->close();
        }
        delete transmitter;
    }

    this->transmitters.clear();
}

// /////////////////////////////////////////////////////////////////////////////

ITransmitter *IOServiceRadio::createAndGetTransmitter(const std::string sender)
{
    // check if a transmitter is already created
    if (this->transmitters.find(sender) != this->transmitters.end())
    {
        return this->transmitters[sender];
    }

    // create the correct transmitter
    ITransmitter *tx = NULL;

#ifdef WITH_ARDUINO
    if (sender.compare("artt") == 0)
    {
        tx = new ArTT(sender, "75232303235351816182", this->io_service);
    }
    if (sender.compare("artp") == 0)
    {
        tx = new ArTP(sender, "9523335323135180F092", this->io_service);
    }
#endif

#ifdef WITH_CRAZYRADIO
    if (sender.compare("crazy") == 0)
    {
        tx = new CrazyRadioTransmitter(sender, this->io_service);
    }
#endif

#ifdef WITH_RASPBERRYPI
    if (sender.compare("raspberrypi") == 0)
    {
        tx = new RpiTX(sender, this->io_service);
    }
#endif

    if (!tx)
    {
        // create the exception string
        std::string ex = boost::str(boost::format("Unable to create transmitter for transmitter type [ %1% ]") % sender);
        // display the exception with logger
        BOOST_LOG_TRIVIAL(error) << ex;
        // trow an exception
        throw RadioException(ex);
    }

    this->transmitters[sender] = tx;

    // get the transmitter
    ITransmitter *transmitter = this->transmitters[sender];
    // connect the fireCopterData to the on copter data from transmitter
    transmitter->onTelemetry.connect(boost::bind(&IOServiceRadio::fireTelemetryData, this, _1));

    return transmitter;
}

void IOServiceRadio::createRadio(RadioConfig &config)
{
    int copterId = config.copterId;

    // check if the copter has already created an radio
    if (this->radios.find(copterId) != this->radios.end())
    {
        // create the exception string
        std::string ex = boost::str(boost::format("Copter [ %1% ] already configured.") % copterId);
        // display the exception with logger
        BOOST_LOG_TRIVIAL(error) << ex;
        // trow an exception
        throw RadioException(ex);
    }

    // get the name of the sender
    std::string sender = config.sender;

    // create or get the transmitter for the configured sender
    ITransmitter *transmitter = this->createAndGetTransmitter(sender);

    // radio which is used for the given copter
    AbstractRadio *radio = NULL;

    if (sender.compare("artt") == 0 || sender.compare("artp") == 0 || sender.compare("raspberrypi") == 0)
    {
        radio = new DSMXRadio(copterId, config);
    }

#ifdef WITH_CRAZYRADIO
    if (sender.compare("crazy") == 0)
    {
        radio = new CrazyRadio(copterId, config.txId);
    }
#endif

    if (!radio)
    {
        // create the exception string
        std::string ex = boost::str(boost::format("Unable to create radio for radio type [ %1% ]") % sender);
        // display the exception with logger
        BOOST_LOG_TRIVIAL(error) << ex;
        // trow an exception
        throw RadioException(ex);
    }

    // add the created radio
    transmitter->addRadio(radio);
    // add the radio to the copter
    this->radios[copterId] = radio;
    // notify about the loaded radio
    this->fireRadioEvent(radio);
}

// /////////////////////////////////////////////////////////////////////////////

void IOServiceRadio::start()
{
    // execute the start of the radio inside the io_service to be thread safe
    this->io_service->post(boost::bind(&IOServiceRadio::doStart, this));
}

void IOServiceRadio::doStart()
{
    // create for each given configuration an radio
    for (auto config : this->configs)
    {
        this->createRadio(config);
    }

    // start the transmitters
    for (auto entry: this->transmitters)
    {
        // get the transmitter
        ITransmitter *transmitter = entry.second;

        // check if he transmitter is open
        if (!transmitter->isOpen())
        {
            // open the transmitter
            transmitter->open();
        }
    }

    // the the radio started
    this->started = true;
}

void IOServiceRadio::stop()
{
    // execute the stop inside the io_service, to be thread safe
    this->io_service->post(boost::bind(&IOServiceRadio::doStop, this));
}

void IOServiceRadio::doStop()
{
    // stop each transmitter
    for (auto entry: this->transmitters)
    {
        // get the transmitter
        ITransmitter *transmitter = entry.second;

        // checks if the transmitter is open
        if (transmitter->isOpen())
        {
            // close the transmitter
            transmitter->close();
        }
    }
    // set the radio stopped
    this->started = false;
}

void IOServiceRadio::join()
{
    // not used, because of the io_service
}

// /////////////////////////////////////////////////////////////////////////////

void IOServiceRadio::fireTelemetryData(Telemetry data)
{
    // fire the copter data
    this->onTelemetry(data);
}

// /////////////////////////////////////////////////////////////////////////////

void IOServiceRadio::fireRadioEvent(AbstractRadio *radio)
{
    // create the RadioEvent
    RadioEvent e;
    e.gear = radio->getGear();
    e.aux1 = radio->getAux1();
    e.txId = radio->getTxId();
    e.copterId = radio->getId();
    e.enabled = radio->isEnabled();
    e.binding = radio->isBinding();
    e.suspended = radio->isSuspended();

    // fire the RadioEvent
    this->onRadioChanged(e);
}

void IOServiceRadio::fireRadioCommand(IRadioCommand *command)
{
    // invoke the fireRadioCommand inside the io_service to be thread safe
    this->io_service->post(boost::bind(&IOServiceRadio::executeCommand, this, command));
}

void IOServiceRadio::executeCommand(IRadioCommand *command)
{
    // get the copter Id
    int copterId = command->getCopterId();

    // check if a radio for the given copter is registered
    if (this->radios.find(copterId) == this->radios.end())
    {
        BOOST_LOG_TRIVIAL(error) << "No radio set for copter [ " << copterId << " ].";
    }
    else
    {
        // get th radio for the copter
        AbstractRadio *radio = this->radios[copterId];
        // execute the command for the found radio
        this->commandExecutor.execute(command, radio);
        // fire a radio event, because the state of the radio changed
        this->fireRadioEvent(radio);
    }

    // remove the command
    delete command;
}

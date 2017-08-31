//
// Created by lasse on 28.09.15.
//

#include "ioserviceradio.h"

IOServiceRadio::IOServiceRadio(std::vector<RadioConfig> configs,
                               boost::shared_ptr<boost::asio::io_service> ioService)
        : configs(configs),
          ioService(ioService)
{
    this->started = false;

    // create tx/module for each config
    for (auto &config : this->configs)
    {
        this->createRadio(config);
    }
}

IOServiceRadio::~IOServiceRadio()
{
    for (auto entry : this->txModules)
    {
        AbstractTxModule *radio = entry.second;

        delete radio;
    }

    this->txModules.clear();

    for (auto entry : this->transmitters)
    {
        ITransmitter *transmitter = entry.second;

        if (transmitter->isOpen())
        {
            transmitter->close();
        }

        delete transmitter;
    }

    this->transmitters.clear();
}

// /////////////////////////////////////////////////////////////////////////////

ITransmitter *IOServiceRadio::createAndGetTransmitter(const RadioConfig &config)
{
    std::string sender = config.transmitter;

    // check if a transmitter is already created
    if (this->transmitters.find(sender) != this->transmitters.end())
    {
        return this->transmitters[sender];
    }

    // create the correct transmitter
    ITransmitter *tx = nullptr;

    if (sender == "simulation")
    {
        tx = new DyscoTx(sender);
    }

#ifdef WITH_ARDUINO
    if (sender == "artt")
    {
        tx = new ArTT(sender, config.serialPort, this->ioService);
    }
    if (sender == "artp")
    {
        tx = new ArTP(sender, config.serialPort, this->ioService);
    }
#endif

#ifdef WITH_CRAZYRADIO
    if (sender == "crazyradio")
    {
        tx = new CrazyRadioTransmitter(sender, this->ioService);
    }
#endif

#ifdef WITH_RASPBERRYPI
    if (sender == "raspberrypi")
    {
        tx = new RpiTX(sender, this->io_service);
    }
#endif

    if (tx == nullptr)
    {
        std::string msg = boost::str(boost::format("Unable to create transmitter for transmitter type [ %1% ]") % sender);
        throw std::runtime_error(msg);
    }

    this->transmitters[sender] = tx;

    // get the transmitter
    ITransmitter *transmitter = this->transmitters[sender];
    // connect the fireCopterData to the on copter data from transmitter
    transmitter->onImuData.connect(boost::bind(&IOServiceRadio::fireImuData, this, _1));

    return transmitter;
}

void IOServiceRadio::createRadio(RadioConfig &config)
{
    int copterId = config.copterId;

    // check if the copter has already created an radio
    if (this->txModules.find(copterId) != this->txModules.end())
    {
        std::string ex = boost::str(boost::format("Copter [ %1% ] already configured.") % copterId);
        throw std::runtime_error(ex);
    }

    // get the name of the transmitter
    std::string sender = config.transmitter;

    // create or get the transmitter for the configured transmitter
    ITransmitter *transmitter = this->createAndGetTransmitter(config);

    // radio which is used for the given copter
    AbstractTxModule *module = nullptr;

    if (sender == "artt" || sender == "artp" || sender == "raspberrypi")
    {
        module = new DSMXModule(copterId, config);
    }

    if (sender == "simulation")
    {
        module = new DyscoModule(copterId, config.transmitter, config.txId);
    }

#ifdef WITH_CRAZYRADIO
    if (sender == "crazyradio")
    {
        module = new CrazyRadioModule(copterId, config.transmitter, config.txId);
    }
#endif

    if (!module)
    {
        std::string msg = boost::str(boost::format("Unable to create module for transmitter type [ %1% ]") % sender);
        throw std::runtime_error(msg);
    }

    // add the created radio
    transmitter->addTxModule(module);
    // add the radio to the copter
    this->txModules[copterId] = module;
    // notify about the loaded radio
    this->fireRadioEvent(module);
}

AbstractTxModule *IOServiceRadio::getTxModule(int copterId)
{
    if (this->txModules.find(copterId) != this->txModules.end())
    {
        return this->txModules[copterId];
    }

    return nullptr;
}

// /////////////////////////////////////////////////////////////////////////////

void IOServiceRadio::start()
{
    // execute the start of the radio inside the io_service to be thread safe
    this->ioService->post(boost::bind(&IOServiceRadio::doStart, this));
}

void IOServiceRadio::doStart()
{
    // create for each given configuration an radio
    /*for (auto config : this->configs)
    {
        this->createRadio(config);
    }*/

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
    this->ioService->post(boost::bind(&IOServiceRadio::doStop, this));
}

void IOServiceRadio::doStop()
{
    // todo gets never called...
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

void IOServiceRadio::fireImuData(ImuData data)
{
    // fire the copter data
    this->onImuData(data);
}

// /////////////////////////////////////////////////////////////////////////////

void IOServiceRadio::fireRadioEvent(AbstractTxModule *radio)
{
    // create the RadioEvent
    RadioEvent e;

    e.copterId = radio->getCopterId();
    e.txName = radio->getTxName();
    e.moduleId = radio->getModuleId();
    e.enabled = radio->isEnabled();
    e.binding = radio->isBinding();
    e.suspended = radio->isSuspended();

    // todo just copy the whole signal
    e.signal[AbstractTxModule::Channel::Throttle] = radio->getThrottle();
    e.signal[AbstractTxModule::Channel::Aileron] = radio->getRoll();
    e.signal[AbstractTxModule::Channel::Elevation] = radio->getPitch();
    e.signal[AbstractTxModule::Channel::Rudder] = radio->getYaw();
    e.signal[AbstractTxModule::Channel::Gear] = radio->getGear();
    e.signal[AbstractTxModule::Channel::Aux1] = radio->getAux1();

    // fire the RadioEvent
    this->onRadioChanged(e);
}

void IOServiceRadio::fireRadioCommand(IRadioCommand *command)
{
    // invoke the fireRadioCommand inside the io_service to be thread safe
    this->ioService->post(boost::bind(&IOServiceRadio::executeCommand, this, command));
}

void IOServiceRadio::executeCommand(IRadioCommand *command)
{
    // get the copter Id
    int copterId = command->getCopterId();

    // check if a radio for the given copter is registered
    if (this->txModules.find(copterId) == this->txModules.end())
    {
        BOOST_LOG_TRIVIAL(error) << "No transmitter module set for copter [ " << copterId << " ].";
    }
    else
    {
        // get the radio for the copter
        AbstractTxModule *module = this->txModules[copterId];

        // execute the command for the found radio
        this->commandExecutor.execute(command, module);

        // fire a radio event, because the state of the radio changed
        this->fireRadioEvent(module);
    }

    // remove the command
    delete command;
}

//
// Created by lasse on 28.09.15.
//

#include "ioserviceradio.h"

IOServiceRadio::IOServiceRadio(boost::shared_ptr<boost::asio::io_service> io_service)
{
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

void IOServiceRadio::initialize(std::vector<RadioConfiguration> confs)
{
    // invoke the initialize of the radio inside the io_service to be thread safe
    this->io_service->post([this, confs]()
                           {
                               for (auto conf: confs)
                               {
                                   this->createRadio(conf);
                               }
                           });
}

ITransmitter *IOServiceRadio::createAndGetTransmitter(const std::string sender)
{
    // check if a transmitter is already created
    if (this->transmitters.find(sender) != this->transmitters.end())
    {
        return this->transmitters[sender];
    }

    // create the corrent transmitter
    if (sender.compare("artt") == 0)
    {
        this->transmitters[sender] = new ArTT(sender, "75232303235351816182", this->io_service);
    } else if (sender.compare("artp") == 0)
    {
        this->transmitters[sender] = new ArTP(sender, "9523335323135180F092", this->io_service);
    } else if (sender.compare("crazy") == 0)
    {
        this->transmitters[sender] = new CrazyRadioTransmitter(sender, this->io_service);
    } else
    {
        // create the exception string
        std::string ex = str(boost::format("can't create transmitter for unknown sender type [ %1% ]") % sender);
        // display the exception with logger
        //BOOST_LOG_TRIVIAL(error) << ex;
        // trow an exception
        throw RadioException(ex);
    }

    // get the transmitter
    return this->transmitters[sender];
}

void IOServiceRadio::createRadio(RadioConfiguration &conf)
{
    int copterId = conf.copterId;

    // check if the copter has already created an radio
    if (this->radios.find(copterId) != this->radios.end())
    {
        // create the exception string
        std::string ex = str(boost::format("copter [ %1% ] already configured") % copterId);
        // display the exception with logger
        //BOOST_LOG_TRIVIAL(error) << ex;
        // trow an exception
        throw RadioException(ex);
    }

    // get the name of the sender
    std::string sender = conf.sender;
    // create or get the transmitter for the configured sender
    ITransmitter *transmitter = this->createAndGetTransmitter(sender);

    // radio which is used for the given copter
    AbstractRadio *radio = NULL;
    // create the Radio for the copter
    if (sender.compare("crazy") == 0)
    {
        // create a new radio for the crazy radio
        radio = new CrazyRadio(copterId, conf.txId);
    } else
    {
        // create a new radio for the given copter
        radio = new DSMXRadio(copterId, conf.txId);
    }
    // add the created radio
    transmitter->addRadio(radio);
    // add the radio to the copter
    this->radios[copterId] = radio;
    // notify about the loaded radio
    this->fireRadioEvent(radio);
}

void IOServiceRadio::start()
{
    // execute the start of the radio inside the io_service to be thread safe
    this->io_service->post([this]()
                           {
                               // open all registered transmitters
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
                           });
}

void IOServiceRadio::stop()
{
    // execute the stop inside the io_service, to be thread safe
    this->io_service->post([this]()
                           {
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
                           });
}

// ############################################################################################

void IOServiceRadio::fireRadioEvent(AbstractRadio *radio)
{
    // create the RadioEvent
    RadioEvent e;
    e.ch5 = radio->getCh5();
    e.ch6 = radio->getCh6();
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
    this->io_service->post([this, command]()
                           {
                               // get the copter Id
                               int copterId = command->getCopterId();

                               // check if a radio for the given copter is registered
                               if (this->radios.find(copterId) == this->radios.end())
                               {
                                   // remove the command
                                   delete command;

                                   BOOST_LOG_TRIVIAL(error) << "no radio for copter [ " << copterId << " ]";
                               } else
                               {
                                   // get th radio for the copter
                                   AbstractRadio *radio = this->radios[copterId];
                                   // execute the command for the found radio
                                   this->commandExecutor.execute(command, radio);
                                   // fire a radio event, because the state of the radio changed
                                   this->fireRadioEvent(radio);
                               }
                           });
}
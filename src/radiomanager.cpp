//
// Created by lasse on 28.09.15.
//

#include "radiomanager.h"

RadioManager::RadioManager()
{
    this->crazyRadioThread = NULL;
    this->initializeCommands();
}

RadioManager::~RadioManager()
{
    for (auto entry: this->radios)
    {
        AbstractRadio *radio = entry.second;
        delete radio;
    }

    this->radios.clear();

    for (auto entry : this->transmitters)
    {
        ArXX *transmitter = entry.second;
        delete transmitter;
    }

    this->transmitters.clear();
}

void RadioManager::initializeCommands()
{
    this->commands[RadioCommandType::ARM] = boost::bind(&RadioManager::onArmCommand, this, _1);
    this->commands[RadioCommandType::BIND_TRANSMITTER] = boost::bind(&RadioManager::onBindTransmitterCommand, this, _1);
    this->commands[RadioCommandType::CONTROL] = boost::bind(&RadioManager::onControlCommand, this, _1);
    this->commands[RadioCommandType::DISARM] = boost::bind(&RadioManager::onDisarmCommand, this, _1);
    this->commands[RadioCommandType::RESUME_COPTER] = boost::bind(&RadioManager::onResumeCopterCommand, this, _1);
    this->commands[RadioCommandType::SUSPEND_COPTER] = boost::bind(&RadioManager::onSuspendCopterCommand, this, _1);
    this->commands[RadioCommandType::TAKE_OVER] = boost::bind(&RadioManager::onTakeOverCommand, this, _1);
    this->commands[RadioCommandType::TOGGLE_SUSPEND_COPTER] = boost::bind(&RadioManager::onToggleSuspensionCommand, this, _1);
    this->commands[RadioCommandType::TOGGLE_TRANSMITTER] = boost::bind(&RadioManager::onToggleTransmitterCommand, this, _1);
}

void RadioManager::initialize(std::vector<RadioConfiguration> confs)
{
    for (auto conf: confs)
    {
        this->createRadio(conf);
    }
}

void RadioManager::createTransmitter(const std::string sender)
{
    if (sender.compare("artt") == 0)
    {
        this->transmitters[sender] = new ArTT("75232303235351816182");
    } else if (sender.compare("artp") == 0)
    {
        this->transmitters[sender] = new ArTP("9523335323135180F092");
    } else
    {
        // create the exception string
        std::string ex = str(boost::format("can't create transmitter for unknown sender [ %1% ]") % sender);
        // display the exception with logger
        BOOST_LOG_TRIVIAL(error) << ex;
        // trow an exception
        throw RadioException(ex);
    }
}

void RadioManager::createRadio(RadioConfiguration &conf)
{
    int copterId = conf.copterId;

    // check if the copter has already created an radio
    if (this->radios.find(copterId) != this->radios.end())
    {
        // create the exception string
        std::string ex = str(boost::format("copter [ %1% ] already configured") % copterId);
        // display the exception with logger
        BOOST_LOG_TRIVIAL(error) << ex;
        // trow an exception
        throw RadioException(ex);
    }

    // get the name of the sender
    std::string sender = conf.sender;
    // radio which is used for the given copter
    AbstractRadio *radio = NULL;

    if (sender.compare("crazy") == 0)
    {
        // create the crazyRadioThread
        if (this->crazyRadioThread == NULL)
        {
            this->crazyRadioThread = new CrazyRadioThread();
        }

        // create a new radio for the crazy radio
        radio = new CrazyRadio(copterId, conf.txId, sender);
        // insert the Crazy Radio to the CrazyRadioThread
        this->crazyRadioThread->setRadio(radio);
    } else
    {
        // create the transmitter for the given sender
        if (this->transmitters.find(sender) == this->transmitters.end())
        {
            this->createTransmitter(sender);
        }

        // get the transmitter
        ArXX *transmitter = this->transmitters[sender];

        // check if the transmitter has capacity
        if (!transmitter->hasCapacity())
        {
            // create the exception string
            std::string ex = str(boost::format("transimitter with sender [ %1% ] has no more capacity") % sender);
            // display the exception with logger
            BOOST_LOG_TRIVIAL(error) << ex;
            // trow an exception
            throw RadioException(ex);
        }

        // create a new radio for the given copter
        radio = new DSMXRadio(copterId, conf.txId, sender);
        // add rge radio to transmitter
        transmitter->addRadio(radio);
    }
    // add the radio to the copter
    this->radios[copterId] = radio;
}

void RadioManager::start()
{
    // open all registered tansmitters
    for (auto entry: this->transmitters)
    {
        // get the transmitter
        ArXX *transmitter = entry.second;
        // open the transmitter
        if (!transmitter->open())
        {
            // create the exception string
            std::string ex = "fail to open transmitter";
            // display the exception with logger
            BOOST_LOG_TRIVIAL(error) << ex;
            // trow an exception
            throw RadioException(ex);
        }
    }

    // start the CrazyRadioThread if exists
    if (this->crazyRadioThread != NULL)
    {
        this->crazyRadioThread->open();
        this->crazyRadioThread->start();
    }
}

void RadioManager::stop()
{
    // stop the CrazyRadioThread if exists
    if (this->crazyRadioThread != NULL)
    {
        this->crazyRadioThread->stop();
        this->crazyRadioThread->quit();
        this->crazyRadioThread->wait();
    }

    for (auto entry: this->transmitters)
    {
        // get the transmitter
        ArXX *transmitter = entry.second;
        // close the transmitter
        transmitter->close();
    }
}

// ############################################################################################

void RadioManager::fireRadioCommand(IRadioCommand *command)
{
    // get the copter Id
    int copterId = command->getCopterId();

    // check if a radio for the given copter is registered
    if (this->radios.find(copterId) == this->radios.end())
    {
        // remove the command
        delete command;

        BOOST_LOG_TRIVIAL(error) << "no radio for copter [ " << copterId << " ]";
        return;
    }

    // get the command type
    RadioCommandType type = command->getType();

    // check if a command exists for the given RadioCommandType
    if (this->commands.find(type) == this->commands.end())
    {
        // remove the command
        delete command;

        // create the exception string
        std::string ex = str(boost::format("no command for radio-command-type [ %1% ]") % ((int) type));
        // display the exception with logger
        BOOST_LOG_TRIVIAL(error) << ex;
        // trow an exception
        throw RadioException(ex);
    }

    // execute the command for the given command
    this->commands[type](command);
    // remove the command
    delete command;
}

// ############################################################################################

void RadioManager::onArmCommand(IRadioCommand *command)
{
    int copterId = command->getCopterId();

    // check if the radio exists
    if (this->radios.find(copterId) != this->radios.end())
    {
        BOOST_LOG_TRIVIAL(info) << "arm";
        // get the radio
        AbstractRadio *radio = this->radios[copterId];
        // set the arm signal
        radio->setArmSignal();
    }
}

void RadioManager::onBindTransmitterCommand(IRadioCommand *command)
{
    int copterId = command->getCopterId();

    // check if the radio exists
    if (this->radios.find(copterId) != this->radios.end())
    {
        BOOST_LOG_TRIVIAL(info) << "bind tx";
        // get the radio
        AbstractRadio *radio = this->radios[copterId];
        // set the bind signal
        radio->setBindSignal();
    }
}

void RadioManager::onControlCommand(IRadioCommand *command)
{
    int copterId = command->getCopterId();
    //ControlCommand *crtl = NULL;
    if (this->radios.find(copterId) != this->radios.end())
    {
        // get the radio
        AbstractRadio *radio = this->radios[copterId];

        // cast the command to ControlCommand
        if (ControlCommand *crtl = dynamic_cast<ControlCommand *>(command))
        {
            // get the control signals
            cv::Vec4d u = crtl->getSignals();
            BOOST_LOG_TRIVIAL(info) << "control " << u;
            // update the controls
            radio->setControls(u[0], u[1], u[2], u[3]);
        }

    }
}

void RadioManager::onDisarmCommand(IRadioCommand *command)
{
    int copterId = command->getCopterId();

    // check if the radio exists
    if (this->radios.find(copterId) != this->radios.end())
    {
        BOOST_LOG_TRIVIAL(info) << "disarm";
        // get the radio
        AbstractRadio *radio = this->radios[copterId];
        // set the disarm signal
        radio->setDisarmSignal();
    }
}

void RadioManager::onResumeCopterCommand(IRadioCommand *command)
{
    int copterId = command->getCopterId();

    // check if the radio exists
    if (this->radios.find(copterId) != this->radios.end())
    {
        BOOST_LOG_TRIVIAL(info) << "resume";
        // get the radio
        AbstractRadio *radio = this->radios[copterId];
        radio->suspend(false);
    }
}

void RadioManager::onSuspendCopterCommand(IRadioCommand *command)
{
    int copterId = command->getCopterId();

    // check if the radio exists
    if (this->radios.find(copterId) != this->radios.end())
    {
        BOOST_LOG_TRIVIAL(info) << "suspend";
        // get the radio
        AbstractRadio *radio = this->radios[copterId];
        radio->suspend(true);
    }
}

void RadioManager::onTakeOverCommand(IRadioCommand *command)
{ }

void RadioManager::onToggleSuspensionCommand(IRadioCommand *command)
{
    int copterId = command->getCopterId();

    // check if the radio exists
    if (this->radios.find(copterId) != this->radios.end())
    {
        BOOST_LOG_TRIVIAL(info) << "toggle sus";
        // get the radio
        AbstractRadio *radio = this->radios[copterId];
        radio->toggleSuspension();
    }
}

void RadioManager::onToggleTransmitterCommand(IRadioCommand *command)
{
    int copterId = command->getCopterId();

    // check if the radio exists
    if (this->radios.find(copterId) != this->radios.end())
    {
        BOOST_LOG_TRIVIAL(info) << "toggle tx";
        // get the radio
        AbstractRadio *radio = this->radios[copterId];
        radio->toggleSender();
    }
}
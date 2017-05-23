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
 *       @date:   13.05.2014
 *****************************************************/

#include "arxx.h"

ArXX::ArXX(const std::string name, int maxNumberOfRadios,
           std::string serialNumber,
           boost::shared_ptr<boost::asio::io_service> io_service)
{
    this->transmitterName = name;
    this->ioService = io_service;
    this->serialNumber = serialNumber;
    this->maxNumberOfModules = maxNumberOfRadios;
}

ArXX::~ArXX()
{
}

void ArXX::open()
{
    std::string deviceDescription;
    // find the deviceName for the given serial number
    std::string portName = this->findDeviceName(deviceDescription);

    // check if a device name is given
    if (portName.empty())
    {
        std::string ex = boost::str(boost::format("Cannot open device with S/N [ %1% ]") % this->serialNumber);
        BOOST_LOG_TRIVIAL(error) << ex;
        throw RadioException(ex);
    }

    // display found device
    BOOST_LOG_TRIVIAL(info) << "Found device [ " << deviceDescription << " ] on [ " << portName << " ]";

    // create the serial port
    this->serialPort = boost::shared_ptr<boost::asio::serial_port>(new boost::asio::serial_port(*this->ioService));

    // open the device name
    boost::system::error_code ec;
    this->serialPort->open(portName, ec);

    // check if an exception happens
    if (ec)
    {
        // create the exception string
        std::string ex = boost::str(boost::format("Cannot open serial port [ %1% ]: [ %2% ].") % portName % ec.message());
        // display the error
        BOOST_LOG_TRIVIAL(error) << ex;
        // throw an radio exception
        throw RadioException(ex);
    }

    // set the serial port options
    this->serialPort->set_option(boost::asio::serial_port::baud_rate(115200));
    this->serialPort->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
    this->serialPort->set_option(boost::asio::serial_port::character_size(boost::asio::serial_port::character_size(8)));
    this->serialPort->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
    this->serialPort->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));

    // the delay to wait for the serial port, before start reading
    boost::posix_time::milliseconds readDelay(200);
    // create the async deadline_timer
    boost::shared_ptr<boost::asio::deadline_timer> timer(new boost::asio::deadline_timer(*this->ioService));
    // set the wait delay to the timer
    timer->expires_from_now(readDelay);
    // register the async timer to invoke the first reading
    timer->async_wait(boost::bind(&ArXX::invokeReading, this, boost::asio::placeholders::error, timer));
}

// /////////////////////////////////////////////////////////////////////////////

std::string ArXX::findDeviceName(std::string &description)
{
    // make the serialNumber to a QString for equals
    QString serialNumber = QString::fromStdString(this->serialNumber);
    for (auto info: QSerialPortInfo::availablePorts())
    {
        // get the serial number
        QString serialNumber = info.serialNumber();
        // check if the serial number equals
        if (this->serialNumber.compare(serialNumber.toStdString()) == 0)
        {
            // get the description
            description = info.description().toStdString();
            // get the device name
            std::string deviceName = info.portName().toStdString();

            // the platform dependend port name
            std::string realPortName;
#ifdef _WIN32
            realPortName = deviceName;
#elif __APPLE__
            realPortName = "/dev/" + deviceName;
#elif __linux
            realPortName = "/dev/" + deviceName;
#endif

            return realPortName;
        }
    }

    // return empty string
    return std::string();
}

// /////////////////////////////////////////////////////////////////////////////

void ArXX::invokeReading(boost::system::error_code ec,
                         boost::shared_ptr<boost::asio::deadline_timer> timer)
{
    // check if an error happens
    if (ec)
    {
        // create the exception string
        std::string ex = boost::str(boost::format("Failed reading data from serial port with S/N [ %1% ]: [ %2% ].") % this->serialNumber % ec.message());
        // throw an radio exception
        throw RadioException(ex);
    }

    // register the first read callback
    this->readData();
}

void ArXX::readData()
{
    // check if the serial port is open
    if (!this->serialPort->is_open())
    {
        return;
    }

    // read until semicolon
    boost::asio::async_read_until(*this->serialPort, this->response, AXX_DELIMITER,
                                  boost::bind(&ArXX::onDataRead,
                                              this,
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));
}

void ArXX::onDataRead(boost::system::error_code ec, size_t bytes_transferred)
{
    // check if an error happens
    if (ec || !bytes_transferred)
    {
        // create the exception string
        std::string ex = boost::str(boost::format("Failed reading data from serial port with S/N [ %1% ]: [ %2% ].") % this->serialNumber % ec.message());
        // display the error
        BOOST_LOG_TRIVIAL(error) << ex;
        // throw an radio exception
        throw RadioException(ex);
    }

    std::istream istream(&this->response);
    std::string str;
    istream >> str;
    // clear EOF bit
    istream.clear();

    // notify about received message
    this->onData(str);

    // read the next data
    this->readData();
}

void ArXX::writeFrame(const char *frame, size_t length)
{
    // check if the serial port is open
    if (!this->serialPort->is_open())
    {
        return;
    }

    // create the new streambuf
    boost::shared_ptr<boost::asio::streambuf> buf(new boost::asio::streambuf());
    // create the output stream
    std::ostream out(buf.get());
    // write the given frame to the out stream
    out.write(frame, length);

    boost::asio::async_write(*this->serialPort, *buf,
                             boost::bind(&ArXX::onDataWritten,
                                         this,
                                         boost::asio::placeholders::error,
                                         buf,
                                         boost::asio::placeholders::bytes_transferred));
}

void ArXX::onDataWritten(boost::system::error_code ec,
                         boost::shared_ptr<boost::asio::streambuf> buf,
                         size_t bytes)
{
    // check if an error happens
    if (ec)
    {
        // create the exception string
        std::string ex = boost::str(boost::format("Writing to serial with S/N [ %1% ] failed with error code [ %2% ].") % this->serialNumber % ec.message());
        // display the error
        BOOST_LOG_TRIVIAL(error) << ex;
        // throw an radio exception
        throw RadioException(ex);
    }

    // free the buffer
    buf->consume(bytes);
    buf.reset();
}

void ArXX::close()
{
    // check if the serial port is inilized and open
    if (this->serialPort && this->serialPort->is_open())
    {
        // close the serial port
        BOOST_LOG_TRIVIAL(info) << "Closing serial connection.";
        this->serialPort->close();
    }
}

// /////////////////////////////////////////////////////////////////////////////

int ArXX::getMaxNumberOfRadios()
{
    return this->maxNumberOfModules;
}

int ArXX::getNumberOfRadios()
{
    return (int) this->modules.size();
}

void ArXX::addTxModule(AbstractTxModule *txModule)
{
    // get the transmitter id convert to int
    std::string txIdStr = txModule->getModuleId();
    int txId = boost::lexical_cast<int>(txIdStr);

    // check if capacity is left
    if (this->hasCapacity())
    {
        this->modules[txId] = txModule;
    }
    else
    {
        // create the exception string
        std::string ex = boost::str(boost::format("Cannot add txModule with transmitter id [ %1% ] to serial port [ %2% ]. No capacity free.") % txId % this->serialNumber);
        // display the error
        //BOOST_LOG_TRIVIAL(error) << ex;
        // throw an txModule exception
        throw RadioException(ex);
    }
}

bool ArXX::hasCapacity()
{
    return this->modules.size() < this->maxNumberOfModules;
}

void ArXX::suspendAll()
{
    for (auto entry: this->modules)
    {
        AbstractTxModule *radio = entry.second;
        radio->suspend(true);
    }
}

void ArXX::getSuspendedTxs(std::unordered_map<int, bool> &suspended)
{
    for (auto entry: this->modules)
    {
        int txId = entry.first;
        AbstractTxModule *radio = entry.second;
        suspended[txId] = radio->isSuspended();
    }
}

void ArXX::getEnabledStates(std::unordered_map<int, bool> &enableds)
{
    for (auto entry: this->modules)
    {
        int txId = entry.first;
        AbstractTxModule *radio = entry.second;
        enableds[txId] = radio->isEnabled();
    }
}

std::string ArXX::getSerialNumber()
{
    return this->serialNumber;
}

bool ArXX::isOpen()
{
    return this->serialPort && this->serialPort->is_open();
}

std::string ArXX::getName()
{
    return this->transmitterName;
}

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

ArXX::ArXX(std::string name,
           int maxNumberOfRadios,
           std::string serialNumber,
           boost::shared_ptr<boost::asio::io_service> ioService)
        : transmitterName(name),
          maxNumberOfModules(maxNumberOfRadios),
          serialNumber(serialNumber),
          ioService(ioService)
{
}

// /////////////////////////////////////////////////////////////////////////////

std::string ArXX::findPortName(std::string &description)
{
    for (auto const &info: QSerialPortInfo::availablePorts())
    {
        // get the serial number
        std::string sn = info.serialNumber().toStdString();

        if (this->serialNumber == sn)
        {
            description = info.description().toStdString();
            std::string deviceName = info.portName().toStdString();

            // the platform-dependent port name
#ifdef _WIN32
            return deviceName
#else // __APPLE__ and __linux
            return std::string("/dev/") + deviceName;
#endif
        }
    }

    // return empty string
    return std::string();
}

// /////////////////////////////////////////////////////////////////////////////

void ArXX::invokeReading(boost::system::error_code errorCode,
                         boost::shared_ptr<boost::asio::deadline_timer> timer)
{
    if (errorCode)
    {
        std::string msg = boost::str(boost::format("Failed reading data from serial port with S/N [ %1% ]: [ %2% ].") % this->serialNumber % errorCode.message());
        throw RadioException(msg);
    }

    // register the first read callback
    this->readData();
}

void ArXX::readData()
{
    if (!this->serialPort->is_open())
    {
        return;
    }

    // read until delimiter
    boost::asio::async_read_until(*this->serialPort, this->response, AXX_DELIMITER,
                                  boost::bind(&ArXX::onDataRead,
                                              this,
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));
}

void ArXX::onDataRead(boost::system::error_code errorCode, size_t bytes_transferred)
{
    if (errorCode || !bytes_transferred)
    {
        std::string msg = boost::str(boost::format("Failed reading data from serial port with S/N [ %1% ]: [ %2% ].") % this->serialNumber % errorCode.message());
        BOOST_LOG_TRIVIAL(error) << msg;
        throw RadioException(msg);
    }

    std::istream istream(&this->response);
    std::string str;
    istream >> str;

    // clear EOF bit
    istream.clear();

    // notify about received message
    this->onData(str);

    // read on
    this->readData();
}

void ArXX::onDataWritten(boost::system::error_code errorCode,
                         boost::shared_ptr<boost::asio::streambuf> buf,
                         size_t bytes)
{
    if (errorCode)
    {
        std::string msg = boost::str(boost::format("Writing to serial with S/N [ %1% ] failed with error code [ %2% ].") % this->serialNumber % errorCode.message());
        BOOST_LOG_TRIVIAL(error) << msg;
        throw RadioException(msg);
    }

    // free the buffer
    buf->consume(bytes);
    buf.reset();
}

// /////////////////////////////////////////////////////////////////////////////

void ArXX::open()
{
    // find the port by the given serial number
    std::string deviceDescription;
    std::string portName = this->findPortName(deviceDescription);

    if (portName.empty())
    {
        std::string msg = boost::str(boost::format("Cannot open device with S/N [ %1% ]") % this->serialNumber);
        BOOST_LOG_TRIVIAL(error) << msg;
        throw RadioException(msg);
    }

    // display found device
    BOOST_LOG_TRIVIAL(info) << "Found device [ " << deviceDescription << " ] on port [ " << portName << " ]";

    // create and open the serial port
    boost::system::error_code errorCode;
    this->serialPort = boost::shared_ptr<boost::asio::serial_port>(new boost::asio::serial_port(*this->ioService));
    this->serialPort->open(portName, errorCode);

    if (errorCode)
    {
        std::string msg = boost::str(boost::format("Cannot open serial port [ %1% ]: [ %2% ].") % portName % errorCode.message());
        BOOST_LOG_TRIVIAL(error) << msg;
        throw RadioException(msg);
    }

    // set serial port options
    this->serialPort->set_option(boost::asio::serial_port::baud_rate(115200));
    this->serialPort->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
    this->serialPort->set_option(boost::asio::serial_port::character_size(boost::asio::serial_port::character_size(8)));
    this->serialPort->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
    this->serialPort->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));

    // create timer, wait for the arduino to be initialized and invoke first read
    boost::shared_ptr<boost::asio::deadline_timer> timer(new boost::asio::deadline_timer(*this->ioService));
    timer->expires_from_now(boost::posix_time::milliseconds(200));
    timer->async_wait(boost::bind(&ArXX::invokeReading, this, boost::asio::placeholders::error, timer));
}

void ArXX::close()
{
    if (this->serialPort && this->serialPort->is_open())
    {
        BOOST_LOG_TRIVIAL(info) << "Closing serial connection.";
        this->serialPort->close();
    }
}

std::string ArXX::getName()
{
    return this->transmitterName;
}

bool ArXX::isOpen()
{
    return this->serialPort && this->serialPort->is_open();
}

bool ArXX::hasCapacity()
{
    return this->modules.size() < this->maxNumberOfModules;
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
        std::string msg = boost::str(boost::format("Cannot add txModule with transmitter id [ %1% ] to serial port [ %2% ]. No capacity free.") % txId % this->serialNumber);
        BOOST_LOG_TRIVIAL(error) << msg;
        throw RadioException(msg);
    }
}

int ArXX::getNumberOfRadios()
{
    return (int) this->modules.size();
}

int ArXX::getMaxNumberOfRadios()
{
    return this->maxNumberOfModules;
}

std::string ArXX::getSerialNumber()
{
    return this->serialNumber;
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

// /////////////////////////////////////////////////////////////////////////////

void ArXX::writeFrame(const char *frame, size_t length)
{
    if (!this->serialPort->is_open())
    {
        return;
    }

    // create stream buffer
    boost::shared_ptr<boost::asio::streambuf> buf(new boost::asio::streambuf());

    // create output stream
    std::ostream out(buf.get());

    // write given frame to the output stream
    out.write(frame, length);

    // write to serial port
    boost::asio::async_write(*this->serialPort, *buf,
                             boost::bind(&ArXX::onDataWritten,
                                         this,
                                         boost::asio::placeholders::error,
                                         buf,
                                         boost::asio::placeholders::bytes_transferred));
}
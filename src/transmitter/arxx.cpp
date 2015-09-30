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

ArXX::ArXX(const std::string name, int maxNumberOfRadios, std::string serialNumber,
           boost::shared_ptr<boost::asio::io_service> io_service)
{
    this->name = name;
    this->io_service = io_service;
    this->serialNumber = serialNumber;
    this->maxNumberOfRadios = maxNumberOfRadios;
}

void ArXX::open()
{
    std::string deviceDescription;
    // find the deviceName for the given serial number
    std::string deviceName = this->findDeviceName(deviceDescription);

    // check if a device name given
    if (deviceName.empty())
    {
        // create the exception string
        std::string ex = str(boost::format("can't find device with serial number [ %1% ]") % this->serialNumber);
        // display the error
        //BOOST_LOG_TRIVIAL(error) << ex;
        // throw an radio exception
        throw RadioException(ex);
    }
    // display found device
    BOOST_LOG_TRIVIAL(info) << "found device [ " << deviceDescription << " ] on [ " << deviceName << " ]";

    // create the serial port
    this->serialPort = boost::shared_ptr<boost::asio::serial_port>(new boost::asio::serial_port(*this->io_service));

    // open the device name
    boost::system::error_code ec;
    this->serialPort->open(deviceName, ec);

    // check if an exception happens
    if (ec)
    {
        // create the exception string
        std::string ex =
                str(boost::format("can't open serial device [ %1% ], because [ %2% ]") % deviceName % ec.message());
        // display the error
        //BOOST_LOG_TRIVIAL(error) << ex;
        // throw an radio exception
        throw RadioException(ex);
    }

    // set the serial port opetions
    this->serialPort->set_option(boost::asio::serial_port::baud_rate(115200));
    this->serialPort->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
    this->serialPort->set_option(boost::asio::serial_port::character_size(boost::asio::serial_port::character_size(8)));
    this->serialPort->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
    this->serialPort->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));

    // register the read data callback
    this->readData();
}

// #################################################################################

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
            realPortName = "/dev/cu." + deviceName;
#elif __linux
            realPortName = "/dev/" + deviceName;
#endif

            return realPortName;
        }
    }

    // return empty string
    return std::string();
}

// #################################################################################

void ArXX::readData()
{
    // check if the serial port is open
    if (!this->serialPort->is_open())
    {
        return;
    }

    // create the new streambuf
    boost::shared_ptr<boost::asio::streambuf> buf(new boost::asio::streambuf());
    // read until XON
    boost::asio::async_read_until(*this->serialPort, *buf, XON,
                                  boost::bind(&ArXX::onDataRead,
                                              this,
                                              boost::asio::placeholders::error,
                                              buf,
                                              boost::asio::placeholders::bytes_transferred));
}

void ArXX::onDataRead(boost::system::error_code ec, boost::shared_ptr<boost::asio::streambuf> buf, size_t bytes)
{
    // check if an error happens
    if (ec)
    {
        // create the exception string
        std::string ex =
                str(boost::format("fail to read data on serial port with serial number [ %1% ], because [ %2% ]") %
                    this->serialNumber % ec.message());
        // display the error
        //BOOST_LOG_TRIVIAL(error) << ex;
        // throw an radio exception
        throw RadioException(ex);
    }

    // read the next data
    this->readData();

    // check if same bytes read
    if (bytes <= 0)
    {
        return;
    }

    // create the input stream
    std::istream input(buf.get());

    // create the array with the buffer
    char data[bytes];
    // read the bytes from the input stream
    input.read(data, bytes);
    // notify about received data
    this->onData(data, bytes);
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

void ArXX::onDataWritten(boost::system::error_code ec, boost::shared_ptr<boost::asio::streambuf> buf, size_t bytes)
{
    // check if an error happens
    if (ec)
    {
        // create the exception string
        std::string ex =
                str(boost::format("fail to write through the serial port with serial number [ %1% ], because [ %2% ]") %
                    this->serialNumber % ec.message());
        // display the error
        //BOOST_LOG_TRIVIAL(error) << ex;
        // throw an radio exception
        throw RadioException(ex);
    }
    // free the buffer
    buf->consume(bytes);
    buf.reset();
}

void ArXX::close()
{
    // todo turn off relays somehow

    // check if the serial port is inilized and open
    if (this->serialPort && this->serialPort->is_open())
    {
        // close the serial port
        this->serialPort->close();
    }
}

// #################################################################################

int ArXX::getMaxNumberOfRadios()
{
    return this->maxNumberOfRadios;
}

int ArXX::getNumberOfRadios()
{
    return this->radios.size();
}

void ArXX::addRadio(AbstractRadio *radio)
{
    int txId = radio->getTxId();
    // check if capacity is left
    if (this->hasCapacity())
    {
        this->radios[txId] = radio;
    } else
    {
        // create the exception string
        std::string ex = str(
                boost::format("can't add radio with trasmitter id [ %1% ] to serial [ %2% ], because no radio capacity available") %
                txId % this->serialNumber);
        // display the error
        //BOOST_LOG_TRIVIAL(error) << ex;
        // throw an radio exception
        throw RadioException(ex);
    }
}

bool ArXX::hasCapacity()
{
    return this->radios.size() < this->maxNumberOfRadios;
}

void ArXX::suspendAll()
{
    for (auto entry: this->radios)
    {
        AbstractRadio *radio = entry.second;
        radio->suspend(true);
    }
}

void ArXX::getSuspendedTxs(std::unordered_map<int, bool> &suspended)
{
    for (auto entry: this->radios)
    {
        int txId = entry.first;
        AbstractRadio *radio = entry.second;
        suspended[txId] = radio->isSuspended();
    }
}

void ArXX::getEnabledStates(std::unordered_map<int, bool> &enableds)
{
    for (auto entry: this->radios)
    {
        int txId = entry.first;
        AbstractRadio *radio = entry.second;
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
    return this->name;
}
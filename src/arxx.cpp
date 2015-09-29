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

using namespace cv;
using namespace std;

ArXX::ArXX(int maxRadios, string _serialNumber)
{
    maxNumberOfRadios = maxRadios;
    serialPort = 0;
    serialNumber = _serialNumber;
}

ArXX::~ArXX()
{
    if(serialPort->is_open())
    {
        serialPort->close();
    }
}

bool ArXX::open(boost::asio::io_service *io)
{
    serialPort = new boost::asio::serial_port(*io);
    
    boost::system::error_code ec;
    serialPort->open("/dev/cu.usbmodem1421", ec);
    
    if(ec)
    {
        BOOST_LOG_TRIVIAL(error) << ec.message();
        return false;
    }
    
    serialPort->set_option(boost::asio::serial_port::baud_rate(115200));
    serialPort->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
    serialPort->set_option(boost::asio::serial_port::character_size(boost::asio::serial_port::character_size(8)));
    serialPort->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
    serialPort->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
    
    boost::asio::async_read_until(*serialPort, buf, XON, boost::bind(&ArXX::readData, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    
    return true;
}

void ArXX::handle_write(boost::system::error_code ec, size_t bytes)
{
    if(ec)
    {
        BOOST_LOG_TRIVIAL(error) << ec.message();
        return;
    }
    
    // BOOST_LOG_TRIVIAL(info) << bytes << " written.";
}

void ArXX::close()
{
    // todo turn off relays somehow
}

int ArXX::getMaxNumberOfRadios()
{
    return maxNumberOfRadios;
}

int ArXX::getNumberOfRadios()
{
    return radios.size();
}

void ArXX::addRadio(AbstractRadio* radio)
{
    radios.push_back(radio);
}

bool ArXX::hasCapacity()
{
    return radios.size() < maxNumberOfRadios;
}

void ArXX::suspendAll()
{
    for(vector<AbstractRadio*>::iterator it = radios.begin(); it != radios.end(); it++)
    {
        (*it)->suspend(true);
    }
}

void ArXX::getSuspendedTxs(vector<bool>& suspended)
{
    for(vector<AbstractRadio*>::iterator it = radios.begin(); it != radios.end(); it++)
    {
        suspended.push_back((*it)->isSuspended());
    }
}
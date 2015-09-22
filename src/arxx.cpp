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

ArXX::ArXX(int maxRadios, QString _serialNumber)
{
    maxNumberOfRadios = maxRadios;
    serial = 0;
    serialNumber = _serialNumber;
}

ArXX::~ArXX()
{
    if(serial->isOpen())
    {
        serial->close();
    }
}

bool ArXX::open()
{
    bool arduinoFound = false;
    bool success = false;
    
    QString portName;
    QString description;
    
    foreach (const QSerialPortInfo& info, QSerialPortInfo::availablePorts())
    {
        description = info.description();
        
//        cout << "--------------" << endl;
//        cout << info.description().toStdString() << endl;
//        cout << info.hasProductIdentifier() << endl;
//        cout << info.hasVendorIdentifier() << endl;
//        cout << info.manufacturer().toStdString() << endl;
//        cout << info.portName().toStdString() << endl;
//        cout << info.productIdentifier() << endl;
//        cout << info.serialNumber().toStdString() << endl;
//        cout << info.systemLocation().toStdString() << endl;
//        cout << info.vendorIdentifier() << endl;
//        cout << "--------------" << endl;
        
        //if (description.split(" ")[0] == "Arduino")
        if(info.serialNumber() == serialNumber)
        {
            portName = info.portName();
            arduinoFound = true;
            break;
        }
    }
    
    serial = new QSerialPort();
    
    if(arduinoFound)
    {
        cout << portName.toStdString() << " connected." << endl;
        
        QString realPortName;
        
        // todo portname prefix may be fixed in upcoming Qt 5.5
#ifdef _WIN32
        realPortName = portName;
#elif __APPLE__
        realPortName = "/dev/cu." + portName;
#elif __linux
        realPortName = "/dev/" + portName;
#endif
        
        serial->setPortName(realPortName);
        serial->setBaudRate(QSerialPort::Baud115200);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        
        if (serial->open(QIODevice::ReadWrite))
        {
            serial->clear();
            connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
            
            cout << "serial port " << description.toStdString() << " opened." << endl;
            success = true;
        }
    }
    
    return success;
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

bool ArXX::hasCapacity()
{
    return radios.size() < maxNumberOfRadios;
}

void ArXX::addRadio(AbstractRadio* radio)
{
    radios.push_back(radio);
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
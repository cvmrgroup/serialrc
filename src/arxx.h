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

#ifndef ARXX_H
#define ARXX_H

#include <iostream>

#include <opencv2/core.hpp>

#include <QObject>
#include <QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "serialhelper.h"
#include "dsmxradio.h"

/// signal to wait for before sending a new command
#define XON 0x11

/// baud rate used for communication
#define BAUD_RATE B115200

/**
 * @brief Software-side class for communication with the software running on the
 * arduino triple transmission built-in arduino board.
 */
class ArXX : public QObject
{
    Q_OBJECT
    
public:
    
    /**
     * @brief Constructor.
     */
    ArXX(int maxNumberOfRadios, QString serialNumber);
    
    /**
     * @brief Destructor.
     */
    virtual ~ArXX();
    
    /**
     *  @brief initialize the serial port.
     */
    bool open();
    
    /**
     * @brief finalize serial port.
     */
    void close();
    
    int getMaxNumberOfRadios();
    int getNumberOfRadios();
    bool hasCapacity();
    void addRadio(AbstractRadio* radio);
    void suspendAll();
    void getSuspendedTxs(std::vector<bool>& suspended);
    
public slots:
    
    /**
     *  @brief read data from serial port.
     */
    virtual void readData() = 0;
    
protected:
    
    /**
     *  write data to serial port.
     */
    virtual void writeData(unsigned int id = 0) = 0;
    
    /**
     *  Qt's serial port implementation.
     */
    QSerialPort *serial;
    
    QString serialNumber;
    
    /**
     *  max number of radios.
     */
    int maxNumberOfRadios;
    
    /**
     *  the radios
     */
    std::vector<AbstractRadio*> radios;
};

#endif /* ARXX_H */

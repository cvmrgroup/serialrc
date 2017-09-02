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
 *    @version:   2.0
 *       @date:   15.08.2017
 *****************************************************/

#ifndef ICARUS_CRAZYRADIOTRANSMITTER_H
#define ICARUS_CRAZYRADIOTRANSMITTER_H

#include <atomic>
#include <unordered_map>

#include <crazyflie_cpp/Crazyflie.h>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/log/trivial.hpp>
#include <boost/thread/latch.hpp>

#include <opencv2/core.hpp>

#include <mathhelper.h>
#include <timing/clock.h>
#include <radio/imudata.h>
#include <interfaces/i_transmitter.h>

#include "crazyradiomodule.h"

class CrazyRadioTransmitter : public ITransmitter
{
public:

    CrazyRadioTransmitter(std::string name,
                          boost::shared_ptr<boost::asio::io_service> ioService);

private:

    void run();

    void initialize();

    void update();

    void cleanup();

    struct logImu
    {
        float acc_x;
        float acc_y;
        float acc_z;
        float gyro_x;
        float gyro_y;
        float gyro_z;
    } __attribute__((packed));

    struct log2
    {
        float mag_x;
        float mag_y;
        float mag_z;
        float baro_temp;
        float baro_pressure;
        float pm_vbat;
    } __attribute__((packed));

    void onCfEmptyAck(const crtpPlatformRSSIAck *data);

    void onCfImuData(uint32_t time_in_ms, logImu *data);

    void onCfLog2Data(uint32_t time_in_ms, log2 *imu);

    void onLinkQuality(float linkQuality);

public:

    void open() override;

    void close() override;

    std::string getName() override;

    bool isOpen() override;

    bool hasCapacity() override;

    void addTxModule(AbstractTxModule *module) override;

private:

    /// the name of the i_transmitter
    std::string name;

    /// the underlying used io_service
    boost::shared_ptr<boost::asio::io_service> ioService;

    /// flag if the CrazyRadioTransmitter is running
    std::atomic<bool> running;

    /// the radio uri
    std::string radioUri;

    /// the map with radios
    std::unordered_map<std::string, AbstractTxModule *> radios;

    /// the crazyflie copter
    Crazyflie *copter;

    bool enableLogging;
    bool enableLoggingImu;
    bool enableLoggingTemperature;
    bool enableLoggingMagneticField;
    bool enableLoggingPressure;
    bool enableLoggingBattery;

    /// the boost thread for the update
    boost::shared_ptr<boost::thread> thread;
};

#endif //ICARUS_CRAZYRADIOTRANSMITTER_H

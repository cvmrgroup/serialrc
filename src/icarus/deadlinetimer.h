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
/** @copyright    Hochschule RheinMain,
 *                University of Applied Sciences
 *     @author    Marc Lieser
 *    @version    1.0
 *       @date    08.05.2017
*****************************************************/

#ifndef DEADLINETIMER_H
#define DEADLINETIMER_H

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/deadline_timer.hpp>

class DeadlineTimer
{
public:

    DeadlineTimer(double frequency, boost::shared_ptr<boost::asio::io_service> ioService);

    /**
     * Update the timers period.
     * @param period The period in ms.
     */
    void updatePeriod(int period);

    /**
     * Update the timers frequency
     * @param frequency The frequency in s^-1.
     */
    void updateFrequency(double frequency);

    void start();

    void invokeTimer(const std::function<void(const boost::system::error_code &)> &handler);

    void cancel();

private:

    /// the duration of time of one cycle (reciprocal of the frequency)
    int period;

    boost::shared_ptr<boost::asio::io_service> ioService;

    boost::shared_ptr<boost::asio::deadline_timer> timer;

    boost::posix_time::ptime reference;
    boost::posix_time::time_duration elapsed;
    boost::posix_time::time_duration estimated;
};

#endif //DEADLINETIMER_H

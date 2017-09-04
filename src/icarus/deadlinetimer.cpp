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

#include "deadlinetimer.h"

DeadlineTimer::DeadlineTimer(double frequency,
                             boost::shared_ptr<boost::asio::io_service> ioService)
{
    this->updateFrequency(frequency);
    this->ioService = ioService;
    this->timer = boost::shared_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(*this->ioService));
}

void DeadlineTimer::updatePeriod(int period)
{
    this->period = period;
}

void DeadlineTimer::updateFrequency(double frequency)
{
    this->period = (int) round((1. / frequency) * 1000.);
}

void DeadlineTimer::start()
{
    // set reference time
    this->reference = boost::posix_time::microsec_clock::universal_time();
}

void DeadlineTimer::invokeTimer(const std::function<void(const boost::system::error_code &)> &handler)
{
    // get elapsed time
    this->elapsed = boost::posix_time::microsec_clock::universal_time() - this->reference;

    // calculate next invocation time
    boost::posix_time::time_duration nextInvocation = this->estimated - this->elapsed + boost::posix_time::millisec(this->period);

    // invoke immediately if negative
    if (nextInvocation.total_milliseconds() < 0)
    {
        nextInvocation = boost::posix_time::millisec(0);
    }

    // update timer expiration time
    this->timer->expires_from_now(nextInvocation);

    // call handler
    this->timer->async_wait(handler);

    // increment next estimated update time
    this->estimated += boost::posix_time::millisec(this->period);
}

void DeadlineTimer::cancel()
{
    this->timer->cancel();
}

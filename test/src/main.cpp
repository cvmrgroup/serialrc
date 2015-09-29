#include <boost/asio/io_service.hpp>

#include <ioserviceradio.h>

#include "inputmanager.h"

int main(int argc, char *argv[])
{
    // create the io_service
    boost::shared_ptr<boost::asio::io_service> io_service(new boost::asio::io_service());

    RadioConfiguration conf;
    conf.txId = 0;
    conf.copterId = 0;
    conf.sender = "crazy";

    std::vector<RadioConfiguration> confs;
    confs.push_back(conf);

    // #############################################

    // create the radio
    IOServiceRadio radio(io_service);

    // initialize and start the Radio
    radio.initialize(confs);
    radio.start();

    // create and start the Input manager
    InputManager inputManager(0, conf.copterId, &radio, io_service);
    inputManager.start();

    try
    {
        // start the io_service
        io_service->run();
    } catch (std::exception &e)
    {
        BOOST_LOG_TRIVIAL(error) << e.what();
    }

    inputManager.stop();
    radio.stop();

    BOOST_LOG_TRIVIAL(info) << "goodbye";

    return 0;
}
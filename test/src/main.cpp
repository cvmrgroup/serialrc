#include <boost/asio/io_service.hpp>
#include <boost/program_options.hpp>

#include <ioserviceradio.h>
#include <config/configloader.h>

#include "inputmanager.h"
#include "programoptions.h"


int main(int argc, const char *argv[])
{
    // create the io_service
    boost::shared_ptr<boost::asio::io_service> io_service(new boost::asio::io_service());

    // #############################################

    ProgramOptions programOptions;
    // pointer for the program arguments
    boost::shared_ptr<boost::program_options::variables_map> args;
    try
    {
        // parse the program arguments from the given parameters
        args = programOptions.parse(argc, argv);
    } catch (std::exception &e)
    {
        // print help if the arguments failed to parse
        programOptions.print_help();
        // display what went wrong
        std::cout << e.what() << std::endl;
        // stop application execution
        return 1;
    }

    // check if the args are correctly given
    if (!args)
    {
        // if not print help
        programOptions.print_help();
        // stop application execution
        return 2;
    }

    // #############################################

    std::string radio_conf = (*args)["radio_conf"].as<std::string>();
    std::vector<RadioConfiguration> confs = ConfigLoader::loadRadioConfigs(radio_conf);

    // #############################################

    // create the radio
    IOServiceRadio radio(confs, io_service);

    // start the Radio
    radio.start();

    // #############################################

    // create and start the Input manager
    InputManager inputManager(0, confs, &radio, io_service);
    inputManager.start();

    // #############################################

    try
    {
        // start the io_service
        io_service->run();
    } catch (std::exception &e)
    {
        BOOST_LOG_TRIVIAL(error) << e.what();
    }

    // #############################################

    inputManager.stop();
    radio.stop();

    // #############################################

    BOOST_LOG_TRIVIAL(info) << "Goodbye.";

    return 0;
}

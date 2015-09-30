//
// Created by lasse on 25.09.15.
//

#include "programoptions.h"

ProgramOptions::ProgramOptions() : options("usage multiradio_test")
{
    this->initialize_options();
}

void ProgramOptions::initialize_options()
{
    namespace po = boost::program_options;
    this->options.add_options()
            ("help", "")
            ("radio_conf", po::value<std::string>()->required(), "the path to the radio configuration file");
}


boost::shared_ptr<boost::program_options::variables_map> ProgramOptions::parse(int argc, const char **argv)
{
    // parse the given command line into parsed_options
    boost::program_options::parsed_options command_line = boost::program_options::parse_command_line(argc, argv,
                                                                                                     this->options);
    // create the variables_map for the found arguments
    boost::shared_ptr<boost::program_options::variables_map> args(new boost::program_options::variables_map());
    // load the given arguments to the variables_map
    boost::program_options::store(command_line, *args);

    // check if help given
    if ((*args).count("help"))
    {
        // return null
        return NULL;
    }

    // check if required options given
    boost::program_options::notify(*args);

    return args;
}

void ProgramOptions::print_help()
{
    // print the help to the console
    std::cout << this->options << std::endl;
}
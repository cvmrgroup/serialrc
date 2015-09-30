//
// Created by lasse on 25.09.15.
//

#ifndef ICARUS_PROGRAMOPTIONS_H
#define ICARUS_PROGRAMOPTIONS_H

#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>

class ProgramOptions
{
public:

    ProgramOptions();

private:

    void initialize_options();

public:

    boost::shared_ptr<boost::program_options::variables_map> parse(int argc, const char *argv[]);

    void print_help();

private:
    boost::program_options::options_description options;

};

#endif //ICARUS_PROGRAMOPTIONS_H

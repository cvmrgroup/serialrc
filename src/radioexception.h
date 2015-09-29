//
// Created by lasse on 28.09.15.
//

#ifndef ICARUS_RADIOEXCEPTION_H
#define ICARUS_RADIOEXCEPTION_H

#include <string>
#include <exception>

class RadioException : public std::exception
{

public:
    RadioException(const std::string exception);

    virtual const char *what() const throw();

private:

    std::string exception;
};


#endif //ICARUS_RADIOEXCEPTION_H

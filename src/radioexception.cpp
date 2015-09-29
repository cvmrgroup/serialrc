//
// Created by lasse on 28.09.15.
//

#include "radioexception.h"

RadioException::RadioException(const std::string exception)
{
    this->exception = exception;
}

const char *RadioException::what() const throw()
{
    return this->exception.c_str();
}

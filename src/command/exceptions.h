#ifndef COMMAND_EXCEPTIONS_H
#define COMMAND_EXCEPTIONS_H

#include "utility/core.h"

class ParseException : public ButterException
{
    std::string error_string;

public:

    ParseException(const std::string& error_string);
};

class ExecuteException : public ButterException
{
    std::string error_string;

public:

    ExecuteException(const std::string& error_string);
};

#endif
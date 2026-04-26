#include "command/argument/IntArgument.h"

#include <stdexcept>
#include "command/exceptions.h"

Argument::Field IntArgument::parse(std::string token)
{
    try
    {
        Argument::Field output;
        output.int_arg = std::stoi(token);
        return output;
    }
    catch (const std::invalid_argument& e)
    {
        throw ParseException("Invalid integer '" + token + "'");
    }
    catch (const std::out_of_range& e)
    {
        throw ParseException("Invalid integer '" + token + "'");
    }
}
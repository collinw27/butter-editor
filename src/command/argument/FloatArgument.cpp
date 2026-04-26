#include "command/argument/FloatArgument.h"

#include <stdexcept>
#include "command/exceptions.h"

Argument::Field FloatArgument::parse(std::string token)
{
    try
    {
        Argument::Field output;
        output.float_arg = std::stof(token);
        return output;
    }
    catch (const std::invalid_argument& e)
    {
        throw ParseException("Invalid float '" + token + "'");
    }
    catch (const std::out_of_range& e)
    {
        throw ParseException("Invalid float '" + token + "'");
    }
}
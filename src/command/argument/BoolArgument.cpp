#include "command/argument/BoolArgument.h"

#include "command/exceptions.h"
#include "utility/core.h"

Argument::Field BoolArgument::parse(std::string token)
{
    std::string lowercase = to_lower(token);
    Argument::Field output;
    if (lowercase == "true")
        output.int_arg = 1;
    else if (lowercase == "false")
        output.int_arg = 0;
    else
        throw ParseException("Invalid boolean '" + token + "'");
    return output;
}
#ifndef ARGUMENT_H
#define ARGUMENT_H

#include <string>

class CommandResult;
class CommandParser;

class Argument
{
    std::string name;

protected:

    // This struct should really by within CommandResult, but nested
    // types cannot be forward-declared :(
    // This is a sensible compromise that doesn't expose the type
    // within the global namespace

    struct Field
    {
        int int_arg = 0;
        float float_arg = 0.f;
        std::string string_arg = "";
    };

public:

    Argument(std::string name);
    std::string get_name();

    virtual Field parse(std::string token) = 0;

    friend CommandResult;
    friend CommandParser;
};

#endif
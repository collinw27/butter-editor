#ifndef COMMAND_RESULT_H
#define COMMAND_RESULT_H

#include <string>
#include <vector>
#include "command/argument/Argument.h"

// Constructed in-place using friend class CommandParser

class CommandParser;

class CommandResult
{
    bool m_is_valid = false;
    std::string error = "";
    std::string root = "";
    std::vector<Argument::Field> fields {};

public:

    bool is_valid();
    std::string get_root();
    std::string get_error();

    bool get_bool(unsigned arg_index);
    int get_int(unsigned arg_index);
    float get_float(unsigned arg_index);
    std::string get_string(unsigned arg_index);

    friend CommandParser;
};

#endif
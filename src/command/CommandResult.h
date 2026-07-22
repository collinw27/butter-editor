#ifndef COMMAND_RESULT_H
#define COMMAND_RESULT_H

#include <string>
#include <vector>

// Constructed in-place using friend class CommandParser

class CommandParser;

class CommandResult
{
    struct Field
    {
        int int_arg = 0;
        float float_arg = 0.f;
        std::string string_arg = "";
    };

    bool is_valid = false;
    std::string error = "";
    std::string root = "";
    int root_id = -1;
    std::vector<Field> fields {};

public:

    bool check_valid();
    std::string get_root();
    int get_root_id();
    std::string get_error();

    // There is very little type safety when extracting values
    // In practice, this is fine, since the caller of these functions
    // should already know the command signature with 100% certainty

    bool get_bool(unsigned arg_index);
    int get_int(unsigned arg_index);
    float get_float(unsigned arg_index);
    std::string get_string(unsigned arg_index);

    friend CommandParser;
};

#endif
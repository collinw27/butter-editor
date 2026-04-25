#ifndef COMMAND_RESULT_H
#define COMMAND_RESULT_H

#include <string>
#include <vector>

// Constructed in-place using friend class CommandParser

class CommandParser;

class CommandResult
{
    enum class FieldType
    {
        EMPTY,
        BOOL,
        INT,
        FLOAT,
        STRING
    };

    struct Field
    {
        FieldType type;
        int int_arg = 0;
        float float_arg = 0.f;
        std::string string_arg = "";
    };

    bool m_is_valid = false;
    std::string error = "";
    std::string root = "";
    std::vector<Field> fields {};

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
#include "command/CommandResult.h"

#include "utility/core.h"

bool CommandResult::valid()
{
    return is_valid;
}

std::string CommandResult::get_root()
{
    return is_valid ? root : "";
}

int CommandResult::get_root_id()
{
    return is_valid ? root_id : -1;
}

std::string CommandResult::get_error()
{
    return is_valid ? "" : error;
}

bool CommandResult::get_bool(unsigned int arg_index)
{
    if (arg_index >= fields.size())
        throw ButterException("Field out of range");
    Field field = fields.at(arg_index);
    return (field.int_arg != 0);
}

int CommandResult::get_int(unsigned int arg_index)
{
    if (arg_index >= fields.size())
        throw ButterException("Field out of range");
    Field field = fields.at(arg_index);
    return field.int_arg;
}

float CommandResult::get_float(unsigned int arg_index)
{
    if (arg_index >= fields.size())
        throw ButterException("Field out of range");
    Field field = fields.at(arg_index);
    return field.float_arg;
}

std::string CommandResult::get_string(unsigned int arg_index)
{
    if (arg_index >= fields.size())
        throw ButterException("Field out of range");
    Field field = fields.at(arg_index);
    return field.string_arg;
}
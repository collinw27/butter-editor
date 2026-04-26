#include "command/CommandResult.h"

#include "utility/core.h"

bool CommandResult::is_valid()
{
    return m_is_valid;
}

std::string CommandResult::get_root()
{
    return m_is_valid ? root : "";
}

std::string CommandResult::get_error()
{
    return m_is_valid ? "" : error;
}

bool CommandResult::get_bool(unsigned arg_index)
{
    if (arg_index >= fields.size())
        throw ButterException("Field out of range");
    auto field = fields.at(arg_index);
    return (field.int_arg != 0);
}

int CommandResult::get_int(unsigned arg_index)
{
    if (arg_index >= fields.size())
        throw ButterException("Field out of range");
    auto field = fields.at(arg_index);
    return field.int_arg;
}

float CommandResult::get_float(unsigned arg_index)
{
    if (arg_index >= fields.size())
        throw ButterException("Field out of range");
    auto field = fields.at(arg_index);
    return field.float_arg;
}

std::string CommandResult::get_string(unsigned arg_index)
{
    if (arg_index >= fields.size())
        throw ButterException("Field out of range");
    auto field = fields.at(arg_index);
    return field.string_arg;
}
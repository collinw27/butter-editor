#include "command/CommandStructure.h"

#include "utility/core.h"

bool CommandStructure::valid()
{
    return is_valid;
}

unsigned int CommandStructure::get_size()
{
    return tokens.size();
}

CommandStructure::TokenType CommandStructure::get_token_type(unsigned int index)
{
    if (index >= tokens.size())
        throw ButterException("Nonexistent token");
    return tokens.at(index).type;
}

unsigned int CommandStructure::get_token_start(unsigned int index)
{
    if (index >= tokens.size())
        throw ButterException("Nonexistent token");
    return tokens.at(index).start_index;
}

std::string CommandStructure::get_token(unsigned int index)
{
    if (index >= tokens.size())
        throw ButterException("Nonexistent token");
    return tokens.at(index).string;
}
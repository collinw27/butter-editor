#include "command/CommandStructure.h"

#include "utility/core.h"

unsigned CommandStructure::get_size()
{
    return tokens.size();
}

CommandStructure::TokenType CommandStructure::get_token_type(unsigned index)
{
    if (index >= tokens.size())
        throw ButterException("Nonexistent token");
    return tokens.at(index).type;
}

unsigned CommandStructure::get_token_start(unsigned index)
{
    if (index >= tokens.size())
        throw ButterException("Nonexistent token");
    return tokens.at(index).start_index;
}

std::string CommandStructure::get_token(unsigned index)
{
    if (index >= tokens.size())
        throw ButterException("Nonexistent token");
    return tokens.at(index).string;
}
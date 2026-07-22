#ifndef COMMAND_STRUCTURE_H
#define COMMAND_STRUCTURE_H

#include <vector>
#include <string>
#include <optional>

// Instead of actually parsing the command using the
// command definition, this class is responsible for breaking
// the input into tokens and returning a datatype for each

// Constructed in-place using friend class CommandParser

class CommandParser;

class CommandStructure
{
public:

    enum class TokenType
    {
        ROOT,
        BOOL,
        INT,
        FLOAT,
        STRING,
        OTHER,
        INVALID
    };

private:

    struct Token
    {
        TokenType type;
        unsigned start_index;
        std::string string;
    };

    std::vector<Token> tokens {};

    // This class is a little strange compared to CommandResult
    // Since invalid commands still should receive syntax highlighting when possible,
    // errors encountered when parsing the structure don't always exit prematurely
    // If they are fatal, the field is_valid is false.
    // If non-fatal, the error is simply stored in the error field for easy access later

    bool is_valid = true;
    std::optional<std::string> error = std::nullopt;

public:

    bool valid();
    unsigned get_size();
    TokenType get_token_type(unsigned index);
    unsigned get_token_start(unsigned index);
    std::string get_token(unsigned index);

    friend CommandParser;
};

#endif
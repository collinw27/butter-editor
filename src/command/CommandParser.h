#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "command/CommandResult.h"

class CommandParser
{
    class Definition
    {
        std::string root;
        std::vector<Argument*> arguments;
        std::unordered_set<std::string> flags;

        Definition(std::string root);

    public:

        Definition& add_argument(Argument* argument);

        friend CommandParser;
    };

    std::unordered_map<std::string, Definition> defined_commands;

public:

    CommandParser();
    Definition new_command(std::string root);
    void define_command(Definition definition);

    CommandResult parse(std::string source);
};

#endif
#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "command/CommandResult.h"

class CommandParser
{
public:

    enum class ParamType
    {
        BOOL,
        INT,
        FLOAT,
        STRING
    };

private:

    struct Parameter
    {
        std::string name;
        ParamType param_type;
    };

    class Definition
    {
        std::string root;
        std::vector<Parameter> parameters;
        std::unordered_set<std::string> flags;

        Definition(std::string root);

    public:

        Definition& add_parameter(std::string param_name, ParamType param_type);

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
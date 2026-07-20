#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <regex>

#include "command/CommandResult.h"
#include "command/CommandStructure.h"

class CommandParser
{
public:

    enum class ParamType
    {
        BOOL,
        INT,
        U_INT,
        FLOAT,
        U_FLOAT,
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

        Definition(std::string root);

    public:

        Definition& add_parameter(std::string param_name, ParamType param_type);

        friend CommandParser;
    };

    std::unordered_map<std::string, Definition> defined_commands;

    // Stored for use during validation

    std::regex int_regex;
    std::regex float_regex;

public:

    CommandParser();
    Definition new_command(std::string root);
    void define_command(Definition definition);    

    CommandResult parse(std::string source);
    CommandStructure parse_structure(std::string source);
    CommandResult::Field parse_arg(ParamType param_type, std::string token);

    // Command info
    
    unsigned get_param_count(std::string root);
    std::string get_param_name(std::string root, unsigned index);
    ParamType get_param_type(std::string root, unsigned index);

    // Argument validation
    // Caveat: Performs validation on PARSED result, not token itself

    static void validate_range(int arg, int min_val, int max_val);
    static void validate_range(float arg, float min_val, float max_val);

private:

    // Lexing helper methods

    void lex_separator(std::stringstream& stream);
    std::string lex_argument(std::stringstream& stream, int& ret_token_start, std::string& ret_error);
};

#endif
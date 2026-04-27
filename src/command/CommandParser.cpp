#include "command/CommandParser.h"

#include <sstream>
#include "utility/core.h"
#include "command/exceptions.h"

CommandParser::Definition::Definition(std::string root)
{
    this->root = root;
}

CommandParser::Definition& CommandParser::Definition::add_parameter(std::string param_name, ParamType param_type)
{
    Parameter new_param {param_name, param_type};
    parameters.push_back(new_param);
    return *this;
}

CommandParser::CommandParser()
{
    int_regex = std::regex("-?[0-9]+");
    float_regex = std::regex("-?[0-9]*(\\.[0-9]+)?((e|E)-?[0-9]+)?");
}

CommandParser::Definition CommandParser::new_command(std::string root)
{
    return CommandParser::Definition(root);
}

void CommandParser::define_command(CommandParser::Definition definition)
{
    if (defined_commands.find(definition.root) != defined_commands.end())
        throw ButterException("Redefinition of command");
    defined_commands.emplace(definition.root, definition);
}

CommandResult CommandParser::parse(std::string source)
{
    try
    {

        // For now, split at spaces
        // In the future, will allow parenthesis & quotes

        std::vector<std::string> tokens {};
        std::string buffer;
        std::stringstream stream {source};
        while (std::getline(stream, buffer, ' '))
        {
            tokens.push_back(buffer);
        }

        if (tokens.size() == 0)
            throw ParseException("Empty command");

        // Initialize the result structure
        // Other properties will be initialized later

        CommandResult result {};
        result.is_valid = true;

        // Get the signature of this command
        
        std::string root = tokens.at(0);
        result.root = root;
        std::vector<Parameter> parameters {};
        auto command_definition = defined_commands.find(root);
        if (command_definition == defined_commands.end())
            throw ParseException("Unknown command '" + root + "'");
        parameters = command_definition->second.parameters;

        // Validate the number and type of arguments

        std::vector<CommandResult::Field> result_fields {};
        if (parameters.size() != tokens.size() - 1)
            throw ParseException("Invalid number of arguments");
        
        for (int i = 0; i < parameters.size(); ++i)
        {
            std::string token = tokens.at(1 + i);
            ParamType param_type = parameters.at(i).param_type;
            result.fields.push_back(parse_arg(param_type, token));
        }

        return result;
    }
    catch (ParseException error)
    {
        CommandResult result {};
        result.is_valid = false;
        result.error = error.message();
        return result;
    }
}

// This is done with a long boilerplate function
// I know this would typically be done with polymorphism via a Parameter
// class, but that seems like overkill for a single method
// This also sidesteps the need to define parameters by pointer

// One advantage of a polymorphic class would be the ability to add qualifiers
// to the results, but CommandParser comes with built-in methods for validation
// that can be called AFTER the command is parsed

CommandResult::Field CommandParser::parse_arg(ParamType param_type, std::string token)
{
    CommandResult::Field field {};
    std::string lowercase_token = to_lower(token);

    switch (param_type)
    {

    case ParamType::BOOL:
        if (lowercase_token == "true")
            field.int_arg = 1;
        else if (lowercase_token == "false")
            field.int_arg = 0;
        else
            throw ParseException("Invalid boolean '" + token + "'");

    break;
    case ParamType::INT:
    case ParamType::U_INT:

        try
        {
            // An additional validation step takes place since C++'s
            // std::stoi() is way to lenient for our usage
            
            if (!std::regex_match(token, int_regex))
                throw ParseException("Invalid integer '" + token + "'");
            field.int_arg = std::stoi(token);
            if (param_type == ParamType::U_INT && field.int_arg < 0)
                throw ParseException("Integer out of range: " + std::to_string(field.int_arg) + " < 0");
        }
        catch (const std::invalid_argument& e)
        {
            throw ParseException("Invalid integer '" + token + "'");
        }
        catch (const std::out_of_range& e)
        {
            throw ParseException("Invalid integer '" + token + "'");
        }

    break;
    case ParamType::FLOAT:
    case ParamType::U_FLOAT:

        try
        {
            // An additional validation step takes place since C++'s
            // std::stof() is way to lenient for our usage
            
            if (!std::regex_match(token, float_regex))
                throw ParseException("Invalid float '" + token + "'");
            field.float_arg = std::stof(token);
            if (param_type == ParamType::U_FLOAT && field.float_arg < 0)
                throw ParseException("Float out of range: " + std::to_string(field.float_arg) + " < 0.0");
        }
        catch (const std::invalid_argument& e)
        {
            throw ParseException("Invalid float '" + token + "'");
        }
        catch (const std::out_of_range& e)
        {
            throw ParseException("Invalid float '" + token + "'");
        }
    break;
    case ParamType::STRING:

        field.string_arg = token;

    break;
    }
    return field;
}

void CommandParser::validate_range(int arg, int min_val, int max_val)
{
    if (arg < min_val)
        throw ParseException((std::stringstream("Integer out of range: ") << arg << " < " << min_val).str());
    if (arg > min_val)
        throw ParseException((std::stringstream("Integer out of range: ") << arg << " > " << max_val).str());
}

void CommandParser::validate_range(float arg, float min_val, float max_val)
{
    if (arg < min_val)
        throw ParseException((std::stringstream("Float out of range: ") << arg << " < " << min_val).str());
    if (arg > min_val)
        throw ParseException((std::stringstream("Float out of range: ") << arg << " > " << max_val).str());
}
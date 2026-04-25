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
        result.m_is_valid = true;

        // Get the signature of this command
        
        std::string root = tokens.at(0);
        result.root = root;
        std::vector<CommandParser::Parameter> parameters {};
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
            std::string lowercase_token = to_lower(token);
            CommandParser::ParamType param_type = parameters.at(i).param_type;
            CommandResult::Field field {};

            if (param_type == CommandParser::ParamType::BOOL)
            {
                field.type = CommandResult::FieldType::BOOL;
                if (lowercase_token == "true")
                    field.int_arg = 1;
                else if (lowercase_token == "false")
                    field.int_arg = 0;
                else
                    throw ParseException("Invalid boolean '" + token + "'");
            }
            else
            {
                field.type = CommandResult::FieldType::INT;
                field.int_arg = 0;
            }
            result.fields.push_back(field);
        }

        return result;
    }
    catch (ParseException error)
    {
        CommandResult result {};
        result.m_is_valid = false;
        result.error = error.message();
        return result;
    }
}
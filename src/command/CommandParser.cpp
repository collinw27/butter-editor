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
        if (source.length() == 0)
            throw ParseException("Empty command");
        if (source.at(0) == ' ')
            throw ParseException("Unexpected leading whitespace");

        // Split at spaces
        // Quotes allow grouping strings together
        // Quotes allow with them escape sequences \' \" \\
        // In the future, will allow parenthesis for mathematical expressions

        std::vector<std::string> tokens {};
        std::stringstream stream {source};
        while (true)
        {
            if (stream.eof())
                break;
            tokens.push_back(lex_argument(stream));
            lex_separator(stream);
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
            throw ParseException("Incorrect number of arguments");
        
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
    if (arg > max_val)
        throw ParseException((std::stringstream("Integer out of range: ") << arg << " > " << max_val).str());
}

void CommandParser::validate_range(float arg, float min_val, float max_val)
{
    if (arg < min_val)
        throw ParseException((std::stringstream("Float out of range: ") << arg << " < " << min_val).str());
    if (arg > max_val)
        throw ParseException((std::stringstream("Float out of range: ") << arg << " > " << max_val).str());
}

void CommandParser::lex_separator(std::stringstream& stream)
{
    char first = stream.get();
    if (!stream.eof() && first != ' ')
        throw ParseException("Missing separation");
    while (true)
    {
        char c = stream.peek();
        if (stream.eof() || c != ' ')
            break;
        stream.get();
    }
}

std::string CommandParser::lex_argument(std::stringstream& stream)
{
    std::string output = "";
    char start = stream.get();

    // Lex quote

    if (start == '"' || start == '\'')
    {
        while (true)
        {
            char c = stream.get();
            if (stream.eof())
                throw ParseException("Unclosed quote");
            else if (c == start)
                break;
            else if (c == '\\')
                output += lex_escape(stream);
            else
                output += c;
        }
    }

    // Lex non-quote
    
    else
    {
        output += start;
        while (!stream.eof())
        {
            char c = stream.peek();
            if (stream.eof() || c == ' ')
                break;
            stream.get();
            output += c;
        }
    }
    return output;
}

std::string CommandParser::lex_escape(std::stringstream& stream)
{
    char c = stream.get();
    if (stream.eof())
        throw ParseException("Invalid escape, unexpected end");
    if (c != '\\' && c != '\"' && c != '\'')
        throw ParseException(std::string("Invalid escape '\\") + c + "'");
    return std::string(1, c);
}
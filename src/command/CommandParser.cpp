#include "command/CommandParser.h"

#include <sstream>
#include "utility/core.h"
#include "command/exceptions.h"

CommandParser::Definition::Definition(std::string root, int id)
{
    this->root = root;
    this->id = id;
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

CommandParser::Definition CommandParser::new_command(std::string root, int id)
{
    return CommandParser::Definition(root, id);
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
        CommandStructure structure = parse_structure(source);
        if (structure.error.has_value())
            throw ParseException(structure.error.value());
            
        std::string root = structure.tokens.at(0).string;
        auto command_definition = defined_commands.find(root);
        if (command_definition == defined_commands.end())
            throw ParseException("Unknown command '" + root + "'");
        std::vector<Parameter>& parameters = command_definition->second.parameters;

        CommandResult result {};
        result.is_valid = true;
        result.root = structure.get_token(0);
        result.root_id = command_definition->second.id;

        for (int i = 0; i < parameters.size(); ++i)
        {
            std::string token = structure.tokens.at(1 + i).string;
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

CommandStructure CommandParser::parse_structure(std::string source)
{
    // Fatal errors throw an exception and exit prematurely
    // Non-fatal errors simply store the error

    // Workflow:
    // 1) Fatal error if pure nonsense (ex. empty command)
    // 2) Lex/parse into token instances
    //    - This step includes per-token parsing errors (ex. unclosed quote)
    // 3) Resolve command signature, creating a fatal error if nonexistent
    // 4) Associate each token with its given type, preserving any errors from before
    // 5) (Optionally) call parse_arg() on each token to see if it is valid

    try
    {
        if (source.length() == 0)
            throw ParseException("Empty command");
        if (source.at(0) == ' ')
            throw ParseException("Unexpected leading whitespace");

        // Initialize the result structure
        // Other properties will be initialized later

        CommandStructure result {};
        result.is_valid = true;

        // Split at spaces
        // Quotes allow grouping strings together
        // Quotes allow with them escape sequences \' \" \\
        // In the future, will allow parenthesis for mathematical expressions

        std::stringstream stream {source};
        int ret_token_start = 0;
        std::string ret_error = "";
        while (true)
        {
            if (stream.eof())
                break;
            CommandStructure::Token new_token {};
            new_token.string = lex_argument(stream, ret_token_start, ret_error);
            new_token.start_index = ret_token_start;
            if (!ret_error.empty())
                result.error = result.error.has_value() ? result.error : ret_error;
            new_token.type = ret_error.empty() ? CommandStructure::TokenType::OTHER : CommandStructure::TokenType::INVALID;
            result.tokens.push_back(new_token);
            lex_separator(stream);
        }

        if (result.tokens.size() == 0)
            throw ParseException("Empty command");

        // Get the signature of this command
        
        std::string root = result.tokens.at(0).string;
        auto command_definition = defined_commands.find(root);
        if (command_definition == defined_commands.end())
            throw ParseException("Unknown command '" + root + "'");
        std::vector<Parameter>& parameters = command_definition->second.parameters;

        // Associate the defined type with each token
        // Extra tokens will be given an error type (non-fatal)

        std::vector<CommandResult::Field> result_fields {};
        if (parameters.size() > result.tokens.size() - 1)
            result.error = result.error.has_value() ? result.error : "Too few arguments provided";
        if (parameters.size() < result.tokens.size() - 1)
            result.error = result.error.has_value() ? result.error : "Too many arguments provided";

        result.tokens.at(0).type = CommandStructure::TokenType::ROOT;
        for (int i = 0; i < result.tokens.size() - 1; ++i)
        {
            CommandStructure::Token& arg_token = result.tokens.at(i + 1);
            if (arg_token.type == CommandStructure::TokenType::INVALID)
                continue;
            arg_token.type = CommandStructure::TokenType::INVALID;
            if (i < parameters.size())
            {
                switch (parameters.at(i).param_type)
                {
                case ParamType::BOOL:
                    arg_token.type = CommandStructure::TokenType::BOOL;
                break;
                case ParamType::INT:
                case ParamType::U_INT:
                    arg_token.type = CommandStructure::TokenType::INT;
                break;
                case ParamType::FLOAT:
                case ParamType::U_FLOAT:
                    arg_token.type = CommandStructure::TokenType::FLOAT;
                break;
                case ParamType::STRING:
                    arg_token.type = CommandStructure::TokenType::STRING;
                break;
                }
                try
                {
                    parse_arg(parameters.at(i).param_type, arg_token.string);
                }
                catch (ParseException)
                {
                    arg_token.type = CommandStructure::TokenType::INVALID;
                }
            }
        }

        return result;
    }
    catch (ParseException error)
    {
        CommandStructure result {};
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

        if (token.length() == 0)
            throw ParseException("Invalid string (empty)");
        if (token.at(0) == '"' || token.at(0) == '\'')
        {
            if (token.at(0) != token.at(token.length() - 1))
                throw ParseException("Invalid string (mismatched quotes)");
            token = token.substr(1, token.length() - 2);
            field.string_arg = "";
            bool escaped = false;
            for (char c : token)
            {
                if (escaped)
                {
                    if (c != '\\' && c != '\"' && c != '\'')
                        throw ParseException(std::string("Invalid escape '\\") + c + "'");
                    field.string_arg += c;
                    escaped = false;
                }
                else if (c == '\\')
                    escaped = true;
                else
                    field.string_arg += c;
            }
        }
        else
            field.string_arg = token;

    break;
    }
    return field;
}

unsigned CommandParser::get_param_count(std::string root)
{
    auto definition = defined_commands.find(root);
    if (definition == defined_commands.end())
        throw ButterException("Nonexistent command");
    return definition->second.parameters.size();
}

std::string CommandParser::get_param_name(std::string root, unsigned index)
{
    auto definition = defined_commands.find(root);
    if (definition == defined_commands.end())
        throw ButterException("Nonexistent command");
    return definition->second.parameters.at(index).name;
}

CommandParser::ParamType CommandParser::get_param_type(std::string root, unsigned index)
{
    auto definition = defined_commands.find(root);
    if (definition == defined_commands.end())
        throw ButterException("Nonexistent command");
    return definition->second.parameters.at(index).param_type;
}

void CommandParser::validate_range(int arg, int min_val, int max_val)
{
    if (arg < min_val)
        throw ExecuteException((std::stringstream{} << "Integer out of range: " << arg << " < " << min_val).str());
    if (arg > max_val)
        throw ExecuteException((std::stringstream{} << "Integer out of range: " << arg << " > " << max_val).str());
}

void CommandParser::validate_range(float arg, float min_val, float max_val)
{
    if (arg < min_val)
        throw ExecuteException((std::stringstream{} << "Float out of range: " << arg << " < " << min_val).str());
    if (arg > max_val)
        throw ExecuteException((std::stringstream{} << "Float out of range: " << arg << " > " << max_val).str());
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

// Additional information is returned with out parameters
// `ret_error` is an empty string if no error was encountered,
// and non-empty if a non-fatal error was encountered

std::string CommandParser::lex_argument(std::stringstream& stream, int& ret_token_start, std::string& ret_error)
{
    std::string output = "";
    char start = stream.get();
    ret_token_start = (int) stream.tellg() - 1;
    ret_error = "";

    // Lex quote

    output += start;
    if (start == '"' || start == '\'')
    {
        while (true)
        {
            // Two cases where we can encounter an error:
            // 1) EOF with unclosed quote
            // 2) EOF with unfinished escape sequence
            // Both of these error out this token without throwing a fatal error

            char c = stream.get();
            if (stream.eof())
            {
                ret_error = "Unclosed quote";
                break;
            }
            else if (c == start)
            {
                output += c;
                break;
            }
            else if (c == '\\')
            {
                output += c;
                c = stream.get();
                if (stream.eof())
                {
                    ret_error = "Invalid escape, unexpected end";
                    break;
                }
                output += c;
            }
            else
                output += c;
        }
    }

    // Lex non-quote
    
    else
    {
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
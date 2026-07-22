#include <gtest/gtest.h>

#include <vector>
#include <iostream>
#include "command/CommandParser.h"

CommandParser* command_parser;

bool test_success(std::string command, std::vector<std::string> expected)
{
    CommandResult result = command_parser->parse(command);
    if (!result.check_valid())
    {
        std::cout << "Unexpected failure: " << result.get_error() << std::endl;
        return false;
    }
    for (int i = 0; i < expected.size(); i += 2)
    {
        std::string dtype = expected.at(i);
        std::string value = expected.at(i+1);
        if (dtype == "int")
        {
            if (result.get_int(i / 2) != std::stoi(value))
            {
                std::cout << "Mismatched int: " << result.get_int(i / 2) << " != " << value << std::endl;
                return false;
            }
        }
        else if (dtype == "float")
        {
            if (result.get_float(i / 2) != std::stof(value))
            {
                std::cout << "Mismatched float: " << result.get_float(i / 2) << " != " << value << std::endl;
                return false;
            }
        }
        else if (dtype == "string")
        {
            if (result.get_string(i / 2) != value)
            {
                std::cout << "Mismatched string: " << result.get_string(i / 2) << " != " << value << std::endl;
                return false;
            }
        }
    }
    return true;
}

bool test_error(std::string command, std::string error_name)
{
    CommandResult result = command_parser->parse(command);
    if (result.get_error() == error_name)
        return true;
    std::cout << "Incorrect error: " << result.get_error() << std::endl;
    return false;
}

bool test_error(std::string command)
{
    CommandResult result = command_parser->parse(command);
    if (!result.check_valid())
        return true;
    std::cout << "Unexpected success" << std::endl;
    return false;
}

TEST(Formatting, MultipleArgs)
{
    ASSERT_TRUE(test_success("fn2 abc def g", {
        "string", "abc", "string", "def", "string", "g"
    }));
}

TEST(Formatting, TooManyArgs)
{
    ASSERT_TRUE(test_error("fn2 a b c d", "Too many arguments provided"));
}

TEST(Formatting, TooFewArgs)
{
    ASSERT_TRUE(test_error("fn2 a b", "Too few arguments provided"));
}

TEST(Formatting, NoArgs)
{
    ASSERT_TRUE(test_error("fn2", "Too few arguments provided"));
}

TEST(Formatting, NoArgsValid)
{
    ASSERT_TRUE(test_success("fn", {}));
}

TEST(Formatting, NoCommand)
{
    ASSERT_TRUE(test_error("", "Empty command"));
}

TEST(Formatting, NoCommandWhitespace)
{
    ASSERT_TRUE(test_error("     ", "Unexpected leading whitespace"));
}

TEST(Formatting, LeadingWhitespace)
{
    ASSERT_TRUE(test_error("   fn1 a", "Unexpected leading whitespace"));
}

TEST(Formatting, ArgumentSeparation)
{
    ASSERT_TRUE(test_success("fn2   a     b c", {
        "string", "a", "string", "b", "string", "c"
    }));
}

TEST(Formatting, InvalidCommand)
{
    ASSERT_TRUE(test_error("aaaaa", "Unknown command 'aaaaa'"));
}

TEST(String, OneArgument)
{
    ASSERT_TRUE(test_success("fn1 abc", {"string", "abc"}));
}

TEST(String, TwoArguments)
{
    ASSERT_TRUE(test_error("fn1 a b", "Too many arguments provided"));
}

TEST(String, QuotedArgument)
{
    ASSERT_TRUE(test_success("fn1 \"a b c\"", {"string", "a b c"}));
}

TEST(String, QuotedArgumentSingle)
{
    ASSERT_TRUE(test_success("fn1 \'a b c\'", {"string", "a b c"}));
}

TEST(String, QuotedArgumentEscaped)
{
    ASSERT_TRUE(test_success("fn1 \"a \\\"b\\\" c\"", {"string", "a \"b\" c"}));
}

TEST(String, QuotedArgumentUnbalanced)
{
    ASSERT_TRUE(test_error("fn1 \"a b c", "Unclosed quote"));
}

TEST(String, QuotedArgumentUnbalancedDifferent)
{
    ASSERT_TRUE(test_error("fn1 \"a b c\'", "Unclosed quote"));
}

TEST(String, MoreEscapes)
{
    ASSERT_TRUE(test_success("fn1 'a\\\"\\\'\\\\'", {"string", "a\"\'\\"}));
}

TEST(String, InvalidEscape)
{
    ASSERT_TRUE(test_error("fn1 '\\a'", "Invalid escape '\\a'"));
}

TEST(Integer, Basic)
{
    ASSERT_TRUE(test_success("fn3 1234", {"int", "1234"}));
}

TEST(Integer, BasicUnsigned)
{
    ASSERT_TRUE(test_success("fn4 1234", {"int", "1234"}));
}

TEST(Integer, Negative)
{
    ASSERT_TRUE(test_success("fn3 -1234", {"int", "-1234"}));
}

TEST(Integer, NegativeUnsigned)
{
    ASSERT_TRUE(test_error("fn4 -1234", "Integer out of range: -1234 < 0"));
}

TEST(Integer, InvalidForm1)
{
    ASSERT_TRUE(test_error("fn3 --1234", "Invalid integer '--1234'"));
}

TEST(Integer, InvalidForm2)
{
    ASSERT_TRUE(test_error("fn3 +1234", "Invalid integer '+1234'"));
}

TEST(Integer, InvalidForm3)
{
    ASSERT_TRUE(test_error("fn3 1234-", "Invalid integer '1234-'"));
}

TEST(Integer, InvalidForm4)
{
    ASSERT_TRUE(test_error("fn4 12.34", "Invalid integer '12.34'"));
}

TEST(Integer, InvalidForm5)
{
    ASSERT_TRUE(test_error("fn4 1234e1", "Invalid integer '1234e1'"));
}

TEST(Integer, InvalidForm6)
{
    ASSERT_TRUE(test_error("fn3 -", "Invalid integer '-'"));
}

TEST(Integer, TooBig)
{
    ASSERT_TRUE(test_error("fn3 123451234512345", "Invalid integer '123451234512345'"));
}

TEST(Float, Basic)
{
    ASSERT_TRUE(test_success("fn5 12.34", {"float", "12.34"}));
}

TEST(Float, BasicUnsigned)
{
    ASSERT_TRUE(test_success("fn6 12.34", {"float", "12.34"}));
}

TEST(Float, Negative)
{
    ASSERT_TRUE(test_success("fn5 -12.34", {"float", "-12.34"}));
}

TEST(Float, NegativeUnsigned)
{
    ASSERT_TRUE(test_error("fn6 -12.34"));
}

TEST(Float, Exponent1)
{
    ASSERT_TRUE(test_success("fn5 1.0e4", {"float", "10000.0"}));
}

TEST(Float, Exponent2)
{
    ASSERT_TRUE(test_success("fn5 1e4", {"float", "10000.0"}));
}

TEST(Float, Exponent3)
{
    ASSERT_TRUE(test_success("fn5 1.0e-4", {"float", "0.0001"}));
}

TEST(Float, Exponent4)
{
    ASSERT_TRUE(test_success("fn6 1e-4", {"float", "0.0001"}));
}

TEST(Float, Exponent5)
{
    ASSERT_TRUE(test_success("fn5 1.0E4", {"float", "10000.0"}));
}

TEST(Float, InvalidForm1)
{
    ASSERT_TRUE(test_error("fn5 --1.0", "Invalid float '--1.0'"));
}

TEST(Float, InvalidForm2)
{
    ASSERT_TRUE(test_error("fn5 1-0", "Invalid float '1-0'"));
}

TEST(Float, InvalidForm3)
{
    ASSERT_TRUE(test_error("fn5 +1.0", "Invalid float '+1.0'"));
}

TEST(Float, InvalidForm4)
{
    ASSERT_TRUE(test_error("fn6 1e2e3", "Invalid float '1e2e3'"));
}

TEST(Float, InvalidForm5)
{
    ASSERT_TRUE(test_error("fn5 1.0.0", "Invalid float '1.0.0'"));
}

TEST(Float, InvalidForm6)
{
    ASSERT_TRUE(test_error("fn5 1.", "Invalid float '1.'"));
}

TEST(Float, InvalidForm7)
{
    ASSERT_TRUE(test_error("fn6 1.0e", "Invalid float '1.0e'"));
}

TEST(Float, InvalidForm8)
{
    ASSERT_TRUE(test_error("fn6 1.0e10aaa", "Invalid float '1.0e10aaa'"));
}

int main(int argc, char** argv)
{
    command_parser = new CommandParser();
    command_parser->define_command(command_parser->new_command("fn"));
    command_parser->define_command(command_parser->new_command("fn1")
        .add_parameter("param1", CommandParser::ParamType::STRING)
    );
    command_parser->define_command(command_parser->new_command("fn2")
        .add_parameter("param1", CommandParser::ParamType::STRING)
        .add_parameter("param2", CommandParser::ParamType::STRING)
        .add_parameter("param3", CommandParser::ParamType::STRING)
    );
    command_parser->define_command(command_parser->new_command("fn3")
        .add_parameter("param1", CommandParser::ParamType::INT)
    );
    command_parser->define_command(command_parser->new_command("fn4")
        .add_parameter("param1", CommandParser::ParamType::U_INT)
    );
    command_parser->define_command(command_parser->new_command("fn5")
        .add_parameter("param1", CommandParser::ParamType::FLOAT)
    );
    command_parser->define_command(command_parser->new_command("fn6")
        .add_parameter("param1", CommandParser::ParamType::U_FLOAT)
    );

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
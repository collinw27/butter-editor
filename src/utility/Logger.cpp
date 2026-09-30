#include "utility/Logger.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include "utility/core.h"
#include "utility/FileManager.h"

LoggerSingleton* LoggerSingleton::singleton_object = nullptr;

LoggerSingleton& Logger()
{
    return *LoggerSingleton::singleton_object;
}

LoggerSingleton::LoggerSingleton()
{
    if (LoggerSingleton::singleton_object != nullptr)
        throw ButterException("Reinitialization of singleton");
    LoggerSingleton::singleton_object = this;
}

LoggerSingleton::~LoggerSingleton()
{
    LoggerSingleton::singleton_object = nullptr;
}

void LoggerSingleton::set_file_output(bool value)
{
    log_to_file = value;
}

void LoggerSingleton::set_log_level(LogLevel new_log_level)
{
    max_log_level = new_log_level;
}

void LoggerSingleton::set_console_output(bool value)
{
    log_to_console = value;
}

void LoggerSingleton::print_startup_message()
{
    std::ofstream file;
    file.open(FileManager().get_data_path("log.txt"), std::ios_base::app | std::ios_base::out);
    if (!file.is_open())
        return;

    // Time printing: https://stackoverflow.com/questions/16357999/current-date-and-time-as-string

    file << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n";
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    file << "* Program started at: " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << '\n';
    file.close();
}

void LoggerSingleton::log(const std::string& text, LogLevel log_level)
{
    _log(text, log_level, log_to_console, log_to_file);
}

void LoggerSingleton::log(const std::stringstream& text, LogLevel log_level)
{
    _log(text.str(), log_level, log_to_console, log_to_file);
}

std::string LoggerSingleton::str(sf::Vector2f vec)
{
    return (std::stringstream{} << "(" << vec.x << ", " << vec.y << ")").str();
}

std::string LoggerSingleton::str(glm::mat3 mat)
{
    float* mat_ptr = (float*)glm::value_ptr(mat);
    std::stringstream output {};
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
            output << mat_ptr[i+j*3] << "  ";
        output << '\n';
    }
    return output.str();
}

std::string LoggerSingleton::str(glm::mat4 mat)
{
    float* mat_ptr = (float*)glm::value_ptr(mat);
    std::stringstream output {};
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
            output << mat_ptr[i+j*4] << "  ";
        output << '\n';
    }
    return output.str();
}

void LoggerSingleton::_log(const std::string& text, LogLevel log_level, bool to_console, bool to_file)
{
    std::string log_message = "";
    if (log_level <= max_log_level)
    {
        switch (log_level)
        {
        case LogLevel::ERR:
            log_message = "[ERROR] ";
        break;
        case LogLevel::WARNING:
            log_message = "[WARN]  ";
        break;
        case LogLevel::INFO:
            log_message = "[INFO]  ";
        break;
        case LogLevel::ALL:
            log_message = "[ALL]   ";
        break;
        }
        log_message += text;

        if (to_console)
        {
            std::cout << log_message << std::endl;
        }
        if (to_file)
        {
            std::ofstream file;
            file.open(FileManager().get_data_path("log.txt"), std::ios_base::app | std::ios_base::out);
            if (!file.is_open())
                _log("Could not open log.txt", LogLevel::ERR, log_to_console, false);
            file << log_message << '\n';
            file.close();
        }
    }
}
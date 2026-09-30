#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <sstream>
#include <cstdarg>

#include <glm/glm.hpp>
#include <SFML/Graphics.hpp>
#include "utility/core.h"

class LoggerSingleton;

LoggerSingleton& Logger();

class LoggerSingleton
{
    static LoggerSingleton* singleton_object;

    LogLevel max_log_level = LogLevel::ALL;
    bool log_to_console = true;
    bool log_to_file = true;

public:

    LoggerSingleton();
    ~LoggerSingleton();

    void set_console_output(bool value);
    void set_file_output(bool value);
    void set_log_level(LogLevel new_log_level);
    
    void print_startup_message();
    void log(const std::string& text, LogLevel log_level = LogLevel::INFO);
    void log(const std::stringstream& text, LogLevel log_level = LogLevel::INFO);

    std::string str(sf::Vector2f vec);
    std::string str(glm::mat3 mat);
    std::string str(glm::mat4 mat);

    friend LoggerSingleton& Logger();

private:

    void _log(const std::string& text, LogLevel log_level, bool to_console, bool to_file);
};

#endif
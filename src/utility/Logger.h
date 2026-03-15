#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <sstream>
#include <cstdarg>

#include <SFML/Graphics.hpp>

class LoggerSingleton;

LoggerSingleton& Logger();

class LoggerSingleton
{
    static LoggerSingleton* singleton_object;

public:

    LoggerSingleton();
    ~LoggerSingleton();

    void log(const std::string& text);
    void log(const std::stringstream& text);

    std::stringstream str(sf::Vector2f vec);

    friend LoggerSingleton& Logger();
};

#endif
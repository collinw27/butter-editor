#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <sstream>
#include <cstdarg>

#include <glm/glm.hpp>
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

    std::string str(sf::Vector2f vec);
    std::string str(glm::mat3 mat);
    std::string str(glm::mat4 mat);

    friend LoggerSingleton& Logger();
};

#endif
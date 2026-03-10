#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <sstream>
#include <cstdarg>

#include <SFML/Graphics.hpp>

class Logger
{
    static Logger* singleton_object;

public:

    static Logger* singleton();
    Logger();
    ~Logger();

    void log(std::stringstream text);

    static std::stringstream str(sf::Vector2f vec);
};

#endif
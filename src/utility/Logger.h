#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <sstream>
#include <cstdarg>

#include <SFML/Graphics.hpp>

using ss = std::stringstream;

class Logger
{
    static Logger *singleton_object;

public:

    static Logger *singleton();
    Logger();
    ~Logger();

    void log(ss text);

    static ss str(sf::Vector2f vec);
};

#endif
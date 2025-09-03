#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <sstream>

using ss = std::stringstream;

class Logger
{
    static Logger *singleton_object;

public:

    static Logger *singleton();
    Logger();
    ~Logger();

    void log(std::stringstream text);
};

#endif
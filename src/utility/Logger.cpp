#include "utility/Logger.h"

LoggerSingleton* LoggerSingleton::singleton_object = nullptr;

LoggerSingleton& Logger()
{
    return *LoggerSingleton::singleton_object;
}

LoggerSingleton::LoggerSingleton()
{
    LoggerSingleton::singleton_object = this;
}

LoggerSingleton::~LoggerSingleton()
{
    LoggerSingleton::singleton_object = nullptr;
}

void LoggerSingleton::log(std::stringstream text)
{
    std::cout << text.str() << std::endl;
}

std::stringstream LoggerSingleton::str(sf::Vector2f vec)
{
    return std::stringstream{} << "(" << vec.x << ", " << vec.y << ")";
}
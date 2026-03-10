#include "utility/Logger.h"

Logger *Logger::singleton_object = nullptr;

Logger *Logger::singleton()
{
    return singleton_object;
}

Logger::Logger()
{
    Logger::singleton_object = this;
}

Logger::~Logger()
{
    Logger::singleton_object = nullptr;
}

void Logger::log(std::stringstream text)
{
    std::cout << text.str() << std::endl;
}

std::stringstream Logger::str(sf::Vector2f vec)
{
    return std::stringstream{} << "(" << vec.x << ", " << vec.y << ")";
}
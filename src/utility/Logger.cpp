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

void Logger::log(ss text)
{
    std::cout << text.str() << std::endl;
}

ss Logger::str(sf::Vector2f vec)
{
    return ss() << "(" << vec.x << ", " << vec.y << ")";
}
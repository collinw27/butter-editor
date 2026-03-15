#include "utility/Logger.h"

#include "utility/Exceptions.h"

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

void LoggerSingleton::log(const std::string& text)
{
    std::cout << text << std::endl;
}

void LoggerSingleton::log(const std::stringstream& text)
{
    std::cout << text.str() << std::endl;
}

std::stringstream LoggerSingleton::str(sf::Vector2f vec)
{
    return std::stringstream{} << "(" << vec.x << ", " << vec.y << ")";
}
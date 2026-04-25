#include "utility/Logger.h"

#include <glm/gtc/type_ptr.hpp>
#include "utility/core.h"

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

std::string LoggerSingleton::str(sf::Vector2f vec)
{
    return (std::stringstream{} << "(" << vec.x << ", " << vec.y << ")").str();
}

std::string LoggerSingleton::str(glm::mat3 mat)
{
    float* mat_ptr = (float*)glm::value_ptr(mat);
    std::stringstream output {};
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
            output << mat_ptr[i+j*3] << "  ";
        output << '\n';
    }
    return output.str();
}

std::string LoggerSingleton::str(glm::mat4 mat)
{
    float* mat_ptr = (float*)glm::value_ptr(mat);
    std::stringstream output {};
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
            output << mat_ptr[i+j*4] << "  ";
        output << '\n';
    }
    return output.str();
}
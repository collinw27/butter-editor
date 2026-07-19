#ifndef CORE_H
#define CORE_H

#include <string>
#include <algorithm>
#include <glm/glm.hpp>
#include "SFML/Graphics.hpp"

// Doesn't validate that a < b

template<typename T>
inline T clamp(T value, T min_val, T max_val)
{
    return std::max(std::min(value, max_val), min_val);
}

inline sf::Vector2f to_sf(glm::vec2 vec)
{
    return sf::Vector2f(vec.x, vec.y);
}

inline sf::Vector3f to_sf(glm::vec3 vec)
{
    return sf::Vector3f(vec.x, vec.y, vec.z);
}

inline glm::vec2 to_gl(sf::Vector2f vec)
{
    return glm::vec2(vec.x, vec.y);
}

inline glm::vec3 to_gl(sf::Vector2f vec, float z)
{
    return glm::vec3(vec.x, vec.y, z);
}

inline glm::vec3 to_gl(sf::Vector3f vec)
{
    return glm::vec3(vec.x, vec.y, vec.z);
}

inline glm::vec4 to_gl(sf::Color col)
{
    return glm::vec4(col.r / 255.0, col.g / 255.0, col.b / 255.0, col.a / 255.0);
}

inline glm::vec4 to_gl(sf::Color col, float alpha)
{
    return glm::vec4(col.r / 255.0, col.g / 255.0, col.b / 255.0, alpha / 255.0);
}

inline glm::vec3 to_gl3(sf::Color col)
{
    return glm::vec3(col.r / 255.0, col.g / 255.0, col.b / 255.0);
}

inline std::string to_lower(std::string source)
{
    std::string lowercase = source;
    std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), [] (unsigned char c) {return std::tolower(c);});
    return lowercase;
}

inline std::string to_upper(std::string source)
{
    std::string uppercase = source;
    std::transform(uppercase.begin(), uppercase.end(), uppercase.begin(), [] (unsigned char c) {return std::toupper(c);});
    return uppercase;
}

inline float lerp(float start, float end, float value)
{
    return (1.0 - value) * start + value * end;
}

inline float inv_lerp(float start, float end, float value)
{
    return (value - start) / (end - start);
}

// Inverse lerp on (old, value) followed by lerp on (new, inverse_lerped_value)

inline float lerp_remap(float old_start, float old_end, float new_start, float new_end, float value)
{
    return lerp(new_start, new_end, inv_lerp(old_start, old_end, value));
}

class ButterException
{
    std::string error_string;

public:

    ButterException(const std::string& error_string);
    std::string message() const;
    void print() const;
};

#endif
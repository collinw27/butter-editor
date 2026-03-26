#include "Math.h"

sf::Vector2f to_sf(glm::vec2 vec)
{
    return sf::Vector2f(vec.x, vec.y);
}

sf::Vector3f to_sf(glm::vec3 vec)
{
    return sf::Vector3f(vec.x, vec.y, vec.z);
}

glm::vec2 to_gl(sf::Vector2f vec)
{
    return glm::vec2(vec.x, vec.y);
}

glm::vec3 to_gl(sf::Vector2f vec, float z)
{
    return glm::vec3(vec.x, vec.y, z);
}

glm::vec3 to_gl(sf::Vector3f vec)
{
    return glm::vec3(vec.x, vec.y, vec.z);
}

glm::vec4 to_gl(sf::Color col)
{
    return glm::vec4(col.r / 255.0, col.g / 255.0, col.b / 255.0, col.a / 255.0);
}

glm::vec4 to_gl(sf::Color col, float alpha)
{
    return glm::vec4(col.r / 255.0, col.g / 255.0, col.b / 255.0, alpha / 255.0);
}
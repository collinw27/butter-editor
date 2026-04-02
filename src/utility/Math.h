#ifndef MATH_H
#define MATH_H

#include <glm/glm.hpp>
#include "SFML/Graphics.hpp"

// Doesn't validate that a < b

template<typename T>
inline T clamp(T value, T min_val, T max_val)
{
    return std::max(std::min(value, max_val), min_val);
}

sf::Vector2f to_sf(glm::vec2 vec);
sf::Vector3f to_sf(glm::vec3 vec);
glm::vec2 to_gl(sf::Vector2f vec);
glm::vec3 to_gl(sf::Vector2f vec, float z);
glm::vec3 to_gl(sf::Vector3f vec);
glm::vec4 to_gl(sf::Color col);
glm::vec4 to_gl(sf::Color col, float alpha);
glm::vec3 to_gl3(sf::Color col);

#endif
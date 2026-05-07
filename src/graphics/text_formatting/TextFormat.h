#ifndef TEXT_FORMAT_H
#define TEXT_FORMAT_H

#include <string>
#include <SFML/Graphics.hpp>

namespace TextFormat
{
    
class Instruction
{
public:

    // For polymorphic class type

    virtual ~Instruction() {}
};

class Color : public Instruction
{
    sf::Color color;

public:

    Color(sf::Color color);
    sf::Color get_color();
};

}

#endif
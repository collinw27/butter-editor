#include "graphics/text_formatting/TextFormat.h"

namespace TextFormat
{

Color::Color(sf::Color color) : color{color} {}

sf::Color Color::get_color()
{
    return color;
}

}
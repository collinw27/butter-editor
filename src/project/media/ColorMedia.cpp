#include "project/media/ColorMedia.h"

ColorMedia::ColorMedia(id_s id, std::string display_name, sf::Color color)
    : MediaItem{id, display_name}
{
    this->color = color;
    thumbnail_tex.reset(new GLTexture(sf::Image(sf::Vector2u(10, 10), this->color)));
}

ColorMedia::ColorMedia(id_s id, std::string display_name, std::ifstream& file)
    : MediaItem{id, display_name}
{
    std::string hex_color;
    file >> hex_color;
    color = hex_to_color("#" + hex_color);
    thumbnail_tex.reset(new GLTexture(sf::Image(sf::Vector2u(10, 10), color)));
}

int ColorMedia::get_media_type()
{
    return (int) MediaType::COLOR;
}

const GLTexture& ColorMedia::get_thumbnail()
{
    return *thumbnail_tex.get();
}

void ColorMedia::save(std::ofstream& file)
{
    file << color_to_hex(color) << " ";
}
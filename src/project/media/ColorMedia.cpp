#include "project/media/ColorMedia.h"

ColorMedia::ColorMedia(id_s id, std::string display_name, sf::Color color)
    : MediaItem{id, display_name}
{
    this->color = color;
}

ColorMedia::ColorMedia(id_s id, std::string display_name, std::ifstream& file)
    : MediaItem{id, display_name}
{
    std::string hex_color;
    file >> hex_color;
    color = hex_to_color("#" + hex_color);
}

int ColorMedia::get_media_type()
{
    return (int) MediaType::COLOR;
}

void ColorMedia::save(std::ofstream& file)
{
    file << color_to_hex(color) << " ";
}

sf::Color ColorMedia::get_color()
{
    return color;
}
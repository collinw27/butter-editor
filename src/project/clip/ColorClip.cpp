#include "project/clip/ColorClip.h"

#include "utility/core.h"

ColorClip::ColorClip(id_s id, VideoTime start_time, VideoTime length, sf::Color color)
    : Clip(start_time, length, id)
{
    this->color = color;
}

ColorClip::ColorClip(id_s id, VideoTime start_time, VideoTime length, std::ifstream& file)
    : Clip(start_time, length, id)
{
    std::string hex_color;
    file >> hex_color;
    color = hex_to_color("#" + hex_color);
}

int ColorClip::get_clip_type()
{
    return (int) ClipType::COLOR;
}

void ColorClip::save(std::ofstream& file)
{
    file << color_to_hex(color) << " ";
}

sf::Color ColorClip::get_color()
{
    return color;
}
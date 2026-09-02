#include "project/clip/ColorClip.h"

#include "utility/core.h"

ColorClip::ColorClip(VideoTime start_time, VideoTime length, uint32_t id, sf::Color color)
    : Clip(start_time, length, id)
{
    this->color = color;
}

ColorClip::ColorClip(VideoTime start_time, VideoTime length, uint32_t id, std::ifstream& file)
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

sf::Color ColorClip::get_color()
{
    return color;
}

void ColorClip::save(std::ofstream& file)
{
    file << color_to_hex(color) << " ";
}
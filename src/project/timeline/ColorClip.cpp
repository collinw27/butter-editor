#include "project/timeline/ColorClip.h"

#include "utility/core.h"

ColorClip::ColorClip(TimelinePos start_time, sf::Color color)
    : TimelineClip(start_time)
{
    this->color = color;
}

ColorClip::ColorClip(TimelinePos start_time, std::ifstream& file)
    : TimelineClip(start_time)
{
    std::string hex_color;
    file >> hex_color;
    color = hex_to_color("#" + hex_color);
}

int ColorClip::get_clip_type()
{
    return (int) TimelineClipType::COLOR;
}

sf::Color ColorClip::get_color()
{
    return color;
}

void ColorClip::save(std::ofstream& file)
{
    file << color_to_hex(color) << " ";
}
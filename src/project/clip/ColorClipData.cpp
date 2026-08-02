#include "project/clip/ColorClipData.h"

#include "utility/core.h"

ColorClipData::ColorClipData(TimelineUnit start_time, TimelineUnit length, sf::Color color)
    : ClipData(start_time, length)
{
    this->color = color;
}

ColorClipData::ColorClipData(TimelineUnit start_time, TimelineUnit length, std::ifstream& file)
    : ClipData(start_time, length)
{
    std::string hex_color;
    file >> hex_color;
    color = hex_to_color("#" + hex_color);
}

int ColorClipData::get_clip_type()
{
    return (int) ClipType::COLOR;
}

sf::Color ColorClipData::get_color()
{
    return color;
}

void ColorClipData::save(std::ofstream& file)
{
    file << color_to_hex(color) << " ";
}
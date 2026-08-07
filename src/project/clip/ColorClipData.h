#ifndef COLOR_CLIP_DATA_H
#define COLOR_CLIP_DATA_H

#include "project/clip/ClipData.h"

#include <fstream>
#include <SFML/Graphics.hpp>

class ColorClipData : public ClipData
{
    sf::Color color;

public:

    ColorClipData(VideoTime start_time, VideoTime length, sf::Color color);
    ColorClipData(VideoTime start_time, VideoTime length, std::ifstream& file);
    
    virtual int get_clip_type() override;
    virtual void save(std::ofstream& file) override;
    
    sf::Color get_color();
};

#endif
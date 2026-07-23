#ifndef COLOR_CLIP_H
#define COLOR_CLIP_H

#include "project/timeline/TimelineClip.h"

#include <fstream>
#include <SFML/Graphics.hpp>

class ColorClip : public TimelineClip
{
    sf::Color color;

public:

    ColorClip(TimelinePos start_time, sf::Color color);
    ColorClip(TimelinePos start_time, std::ifstream& file);
    
    virtual int get_clip_type() override;
    virtual void save(std::ofstream& file) override;
    
    sf::Color get_color();
};

#endif
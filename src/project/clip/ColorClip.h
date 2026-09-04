#ifndef COLOR_CLIP_H
#define COLOR_CLIP_H

#include "project/clip/Clip.h"

#include <fstream>
#include <SFML/Graphics.hpp>

class ColorClip : public Clip
{
    sf::Color color;

public:

    ColorClip(id_s id, VideoTime start_time, VideoTime length, sf::Color color);
    ColorClip(id_s id, VideoTime start_time, VideoTime length, std::ifstream& file);
    
    virtual int get_clip_type() override;
    virtual void save(std::ofstream& file) override;
    
    sf::Color get_color();
};

#endif
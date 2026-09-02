#ifndef COLOR_CLIP_H
#define COLOR_CLIP_H

#include "project/clip/Clip.h"

#include <fstream>
#include <SFML/Graphics.hpp>

class ColorClip : public Clip
{
    sf::Color color;

public:

    ColorClip(VideoTime start_time, VideoTime length, id_s id, sf::Color color);
    ColorClip(VideoTime start_time, VideoTime length, id_s id, std::ifstream& file);
    
    virtual int get_clip_type() override;
    virtual void save(std::ofstream& file) override;
    
    sf::Color get_color();
};

#endif
#ifndef COLOR_CLIP_H
#define COLOR_CLIP_H

#include "project/clip/Clip.h"

#include <memory>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "graphics/texture/GLTexture.h"

class ColorClip : public Clip
{
    sf::Color color;
    std::unique_ptr<GLTexture> thumbnail_tex;

public:

    ColorClip(id_s id, VideoTime start_time, VideoTime length, sf::Color color);
    ColorClip(id_s id, VideoTime start_time, VideoTime length, std::ifstream& file);
    
    virtual ClipType get_clip_type() override;
    virtual const GLTexture* get_thumbnail() override;
    virtual void save(std::ofstream& file) override;
    
    sf::Color get_color();
};

#endif
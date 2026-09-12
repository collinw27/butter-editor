#ifndef COLOR_CLIP_H
#define COLOR_CLIP_H

#include "project/clip/Clip.h"

#include <memory>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "graphics/texture/GLTexture.h"
#include "graphics/GLRectangle.h"

class ColorClip : public Clip
{
    sf::Color color;
    std::unique_ptr<GLTexture> thumbnail_tex;
    std::unique_ptr<GLRectangle> render_rect;

public:

    ColorClip(id_s id, VideoTime start_time, VideoTime length, sf::Color color);
    ColorClip(id_s id, VideoTime start_time, VideoTime length, std::ifstream& file);
    
    virtual ClipType get_clip_type() override;
    virtual const GLTexture* get_thumbnail() override;
    virtual void save(std::ofstream& file) override;
    
    virtual void enter_frame(GLFrameBuffer* buffer) override;
    virtual void exit_frame(GLFrameBuffer* buffer) override;
    virtual void update_frame(GLFrameBuffer* buffer, VideoTime time) override;
    
    sf::Color get_color();
};

#endif
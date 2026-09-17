#ifndef IMAGE_CLIP_H
#define IMAGE_CLIP_H

#include <filesystem>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "project/clip/Clip.h"
#include "project/media/ImageMedia.h"
#include "graphics/GLSprite.h"

class ImageClip : public Clip
{
    id_s image_id;
    ImageMedia* source;
    const GLTexture* render_tex;
    std::unique_ptr<GLSprite> render_sprite;

public:

    ImageClip(id_s id, VideoTime start_time, VideoTime length, ImageMedia* source);
    
    virtual ClipType get_clip_type() override;
    virtual const GLTexture* get_thumbnail() override;
    virtual void save(std::ofstream& file) override;
    
    virtual void enter_frame(GLFrameBuffer* buffer) override;
    virtual void exit_frame(GLFrameBuffer* buffer) override;
    virtual void update_frame(GLFrameBuffer* buffer, VideoTime time) override;
};

#endif
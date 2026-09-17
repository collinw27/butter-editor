#include "project/clip/ImageClip.h"

#include "project/types.h"

ImageClip::ImageClip(id_s id, VideoTime start_time, VideoTime length, ImageMedia* source)
    : Clip(start_time, length, id)
{
    this->source = source;
    render_tex = source->get_texture();
}

ClipType ImageClip::get_clip_type()
{
    return ClipType::IMAGE;
}

const GLTexture* ImageClip::get_thumbnail()
{
    return source->get_thumbnail();
}

void ImageClip::save(std::ofstream& file)
{
    file << source->id << " ";
}

void ImageClip::enter_frame(GLFrameBuffer* buffer)
{
    render_sprite.reset(GLSprite::create(buffer, render_tex));
    render_sprite->set_scale(sf::Vector2f(1, 1) * 1.f);
}

void ImageClip::exit_frame(GLFrameBuffer* buffer)
{
    render_sprite.reset(nullptr);
}

void ImageClip::update_frame(GLFrameBuffer* buffer, VideoTime time)
{
    
}
#include "project/clip/ImageClip.h"

#include "project/types.h"

ImageClip::ImageClip(id_s id, VideoTime start_time, VideoTime length, ImageMedia* source)
    : Clip(start_time, length, id)
{
    this->source = source;
}

ClipType ImageClip::get_clip_type()
{
    return ClipType::IMAGE;
}

const GLTexture* ImageClip::get_thumbnail()
{
    return source->get_thumbnail();
}

void ImageClip::write_frame(GLFrameBuffer* buffer, VideoTime time)
{
    
}

void ImageClip::save(std::ofstream& file)
{
    file << source->id << " ";
}
#ifndef IMAGE_CLIP_H
#define IMAGE_CLIP_H

#include "project/clip/Clip.h"
#include "project/media/ImageMedia.h"

#include <filesystem>
#include <fstream>
#include <SFML/Graphics.hpp>

class ImageClip : public Clip
{
    id_s image_id;
    ImageMedia* source;

public:

    ImageClip(id_s id, VideoTime start_time, VideoTime length, ImageMedia* source);
    
    virtual ClipType get_clip_type() override;
    virtual const GLTexture* get_thumbnail() override;
    virtual void save(std::ofstream& file) override;
};

#endif
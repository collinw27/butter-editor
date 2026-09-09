#ifndef IMAGE_MEDIA_H
#define IMAGE_MEDIA_H

#include <memory>
#include <fstream>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include "project/media/MediaItem.h"

class ImageMedia : public MediaItem
{
    std::filesystem::path image_path;
    sf::Image image;
    std::unique_ptr<GLTexture> image_tex;

public:

    ImageMedia(id_s id, std::string display_name, std::filesystem::path image_path);
    ImageMedia(id_s id, std::string display_name, std::ifstream& file);
    
    virtual MediaType get_media_type();
    virtual const GLTexture* get_thumbnail() override;
    virtual void save(std::ofstream& file) override;
};

#endif
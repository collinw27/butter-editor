#ifndef COLOR_MEDIA_H
#define COLOR_MEDIA_H

#include <fstream>
#include <SFML/Graphics.hpp>
#include "project/media/MediaItem.h"

class ColorMedia : public MediaItem
{
    std::unique_ptr<GLTexture> thumbnail_tex;
    sf::Color color;

public:

    ColorMedia(id_s id, std::string display_name, sf::Color color);
    ColorMedia(id_s id, std::string display_name, std::ifstream& file);
    
    virtual MediaType get_media_type();
    virtual const GLTexture* get_thumbnail() override;
    virtual void save(std::ofstream& file) override;

    sf::Color get_color();
};

#endif
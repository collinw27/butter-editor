#ifndef MEDIA_COLOR_H
#define MEDIA_COLOR_H

#include <fstream>
#include <SFML/Graphics.hpp>
#include "project/media/MediaItem.h"

class ColorMedia : public MediaItem
{
    sf::Color color;

public:

    ColorMedia(id_s id, std::string display_name, sf::Color color);
    ColorMedia(id_s id, std::string display_name, std::ifstream& file);
    
    virtual int get_media_type();
    virtual void save(std::ofstream& file) override;
    sf::Color get_color();
};

#endif
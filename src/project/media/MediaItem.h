#ifndef MEDIA_ITEM_H
#define MEDIA_ITEM_H

#include <SFML/Graphics.hpp>
#include "utility/core.h"
#include "project/types.h"
#include "graphics/GLTexture.h"

class MediaItem
{
public:

    const id_s id;

private:

    std::string display_name;

public:

    MediaItem(id_s id, std::string display_name);
    std::string get_display_name();
    
    // Important: The thumbnail reference returned by `get_thumbnail()`
    // is shared by any other object that displays the thumbnail
    // (ex. clips, drag events)
    // It is therefore very important to maintain the lifetime of the
    // texture throughout the existence of the media in the project

    virtual MediaType get_media_type() = 0;
    virtual const GLTexture* get_thumbnail() = 0;
    virtual void save(std::ofstream& file);
};

#endif
#ifndef MEDIA_ITEM_H
#define MEDIA_ITEM_H

#include "utility/core.h"

// Enum for serializing media instances
// Having to store all subclasses in this file feels weird, but
// an enum is the easiest way to prevent ID collisions

enum class MediaType
{
    EMPTY,
    COLOR
};

class MediaItem
{
public:

    const id_s id;

private:

    std::string display_name;

public:

    MediaItem(id_s id, std::string display_name);
    std::string get_display_name();
    virtual int get_media_type() = 0;

    virtual void save(std::ofstream& file);
};

#endif
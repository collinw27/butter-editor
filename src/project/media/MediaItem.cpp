#include "project/media/MediaItem.h"

MediaItem::MediaItem(id_s id, std::string display_name)
    : id{id}
{
    this->display_name = display_name;
}

std::string MediaItem::get_display_name()
{
    return display_name;
}

void MediaItem::save(std::ofstream& file) {}
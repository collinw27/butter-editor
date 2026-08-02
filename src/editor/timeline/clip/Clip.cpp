#include "editor/timeline/clip/Clip.h"

Clip::Clip(ClipData* clip_data, sf::Color color, GLNode* container)
{
    this->clip_data = clip_data;
    rect.reset(GLRectangle::create(
        container,
        sf::Vector2f(clip_data->get_start_time(), 40),
        sf::Vector2f(clip_data->get_length(), 100)
    ));
    rect->set_fill_color(color);
}

void Clip::delete_clip(Project* project)
{
    
}
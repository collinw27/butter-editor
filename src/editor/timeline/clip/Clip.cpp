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

bool Clip::selected()
{
    return is_selected;
}

void Clip::select(GLNode* container)
{
    if (!is_selected)
    {
        is_selected = true;
        border.reset(GLRectangle::create(
            container,
            rect->get_position() - sf::Vector2f(4, 4),
            rect->get_size() + sf::Vector2f(8, 8)
        ));
        border->set_fill_color(sf::Color{255, 238, 125});
    }
}

void Clip::deselect()
{
    if (is_selected)
    {
        is_selected = false;
        border->get_parent()->remove_child(border.get());
        border.release();
    }
}

void Clip::delete_clip(Project* project)
{
    
}
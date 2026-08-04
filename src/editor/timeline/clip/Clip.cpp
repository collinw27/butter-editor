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
    this->color = color;
}

GLRectangle* Clip::get_rect()
{
    return rect.get();
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

bool Clip::is_time_within(int time)
{
    return (time >= clip_data->get_start_time() && time < clip_data->get_end_time());
}

void Clip::set_hovering(bool hovering)
{
    if (hovering)
        rect->set_fill_color(sf::Color(std::min((int) color.r + 50, 255), std::min((int) color.g + 50, 255), std::min((int) color.b + 50, 255)));
    else
        rect->set_fill_color(color);
}

void Clip::delete_clip(Project* project)
{
    
}
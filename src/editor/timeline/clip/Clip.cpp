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

GLRectangle* Clip::get_border()
{
    return border.get();
}

bool Clip::selected()
{
    return is_selected;
}

void Clip::render_selected(GLNode* container, float t_scale)
{
    if (!is_selected)
    {
        is_selected = true;
        border.reset(GLRectangle::create(
            container,
            rect->get_position() - sf::Vector2f(4 / t_scale, 4),
            rect->get_size() + sf::Vector2f(8 / t_scale, 8)
        ));
        border->set_fill_color(sf::Color{255, 238, 125});
    }
    else
    {
        border->set_position(rect->get_position() - sf::Vector2f(4 / t_scale, 4));
        border->set_size(rect->get_size() + sf::Vector2f(8 / t_scale, 8));
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
        rect->set_fill_color(sf::Color(std::min((int) color.r + 12, 255), std::min((int) color.g + 12, 255), std::min((int) color.b + 12, 255)));
    else
        rect->set_fill_color(color);
}

void Clip::delete_clip(Project* project)
{
    project->delete_clip(clip_data);
}
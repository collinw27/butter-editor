#include "editor/timeline/TimelineClip.h"

TimelineClip::TimelineClip(id_s clip_id, GLNode* container)
    : clip_id{clip_id}
{
    rect.reset(GLRectangle::create(
        container,
        sf::Vector2f(0, 0),
        sf::Vector2f(0, 100)
    ));
}

GLRectangle* TimelineClip::get_rect()
{
    return rect.get();
}

GLRectangle* TimelineClip::get_border()
{
    return border.get();
}

bool TimelineClip::selected()
{
    return is_selected;
}

void TimelineClip::render_selected(GLNode* container, float t_scale)
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

void TimelineClip::deselect()
{
    if (is_selected)
    {
        is_selected = false;
        border->get_parent()->remove_child(border.get());
        border.release();
    }
}

bool TimelineClip::is_time_within(int time)
{
    return (time >= clip_start && time < (clip_start + clip_length));
}

bool TimelineClip::is_start_within(float left, float right)
{
    return (left <= (float) clip_start && right > (float) clip_start);
}

bool TimelineClip::is_end_within(float left, float right)
{
    return (left <= (float) (clip_start + clip_length) && right > (float) (clip_start + clip_length));
}

void TimelineClip::set_hovering(bool hovering)
{
    if (hovering)
        rect->set_fill_color(sf::Color(
            std::min((int) thumbnail_color.r + 12, 255),
            std::min((int) thumbnail_color.g + 12, 255),
            std::min((int) thumbnail_color.b + 12, 255)
        ));
    else
        rect->set_fill_color(thumbnail_color);
}

void TimelineClip::set_clip_start(VideoTime start)
{
    clip_start = start;
    rect->set_position(sf::Vector2f(clip_start, rect->get_position().y));
}

void TimelineClip::set_clip_length(VideoTime length)
{
    clip_length = length;
    rect->set_size(sf::Vector2f(clip_length, rect->get_size().y));
}

void TimelineClip::set_clip_end(VideoTime end)
{
    if (end > clip_start)
        clip_length = end - clip_start;
    rect->set_position(sf::Vector2f(clip_start, rect->get_position().y));
    rect->set_size(sf::Vector2f(clip_length, rect->get_size().y));
}

void TimelineClip::set_thumbnail_color(sf::Color color)
{
    thumbnail_color = color;
    rect->set_fill_color(thumbnail_color);
}
#include "editor/timeline/TimelineClip.h"

#include "editor/timeline/TimelineModule.h"

TimelineClip::TimelineClip(TimelineModule *parent, sf::Color color, float start, float length)
{
    start_time = start;
    end_time = start_time + length;
    this->parent = parent;

    sf::FloatRect bounds = parent->get_clip_bounds(start, length);
    rect = sf::RectangleShape(bounds.size);
    rect.setPosition(bounds.position);
    rect.setFillColor(color);
    rect.setOutlineColor(sf::Color::Yellow);
}

void TimelineClip::draw(sf::RenderWindow &window)
{
    window.draw(rect);
}

void TimelineClip::select()
{
    rect.setOutlineThickness(3);
}

void TimelineClip::deselect()
{
    rect.setOutlineThickness(0);
}

bool TimelineClip::was_clicked(sf::Vector2i cursor_pos)
{
    return rect.getGlobalBounds().contains(static_cast<sf::Vector2f>(cursor_pos));
}

float TimelineClip::get_start()
{
    return start_time;
}

float TimelineClip::get_end()
{
    return end_time;
}

float TimelineClip::get_length()
{
    return end_time - start_time;
}

void TimelineClip::trim_start(float new_start)
{
    start_time = new_start;
    sf::FloatRect bounds = parent->get_clip_bounds(start_time, end_time - start_time);
    rect.setPosition(bounds.position);
    rect.setSize(bounds.size);
}

void TimelineClip::trim_end(float new_end)
{
    end_time = new_end;
    sf::FloatRect bounds = parent->get_clip_bounds(start_time, end_time - start_time);
    rect.setPosition(bounds.position);
    rect.setSize(bounds.size);
}
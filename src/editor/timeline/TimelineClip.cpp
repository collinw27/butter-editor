#include "editor/timeline/TimelineClip.h"

#include "editor/timeline/TimelineModule.h"

TimelineClip::TimelineClip(Node *root_node, sf::Color color, int start, int length)
{
    start_time = start;
    end_time = start_time + length;

    rect = new RectNode(root_node, color, sf::FloatRect(
        sf::Vector2f(start_time * 10.f, 0.f),
        sf::Vector2f(length * 10.f, 80.f)
    ));
    rect->set_outline_color(sf::Color::Yellow);
}

void TimelineClip::draw(sf::RenderWindow &window)
{
    rect->draw(window);
}

void TimelineClip::select()
{
    rect->set_outline_thickness(3);
}

void TimelineClip::deselect()
{
    rect->set_outline_thickness(0);
}

bool TimelineClip::was_clicked(sf::Vector2i cursor_pos)
{
    return rect->contains(static_cast<sf::Vector2f>(cursor_pos));
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
    rect->set_position(sf::Vector2f(start_time * 10.f, 0.f));
    rect->set_size(sf::Vector2f((end_time - start_time) * 10.f, 80.f));
}

void TimelineClip::trim_end(float new_end)
{
    end_time = new_end;
    rect->set_position(sf::Vector2f(start_time * 10.f, 0.f));
    rect->set_size(sf::Vector2f((end_time - start_time) * 10.f, 80.f));
}
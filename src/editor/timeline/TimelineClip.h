#ifndef CLIP_H
#define CLIP_H

#include <SFML/Graphics.hpp>

class TimelineModule;

class TimelineClip
{
    TimelineModule *parent;

    float start_time;
    float end_time;
    sf::RectangleShape rect;

public:

    TimelineClip(TimelineModule *parent, sf::Color color, float start_time, float length);

    void draw(sf::RenderWindow &window);
    void select();
    void deselect();
    bool was_clicked(sf::Vector2i cursor_pos);

    float get_start();
    float get_end();
    float get_length();

    void trim_start(float new_start);
    void trim_end(float new_end);
};

#endif
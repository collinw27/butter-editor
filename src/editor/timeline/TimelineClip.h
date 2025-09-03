#ifndef CLIP_H
#define CLIP_H

#include <SFML/Graphics.hpp>

#include "graphics/RectNode.h"

class TimelineModule;

class TimelineClip
{
    int start_time;
    int end_time;
    RectNode *rect;

public:

    TimelineClip(Node *root_node, sf::Color color, int start_time, int length);

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
#ifndef MOUSE_DRAG_H
#define MOUSE_DRAG_H

#include <SFML/Graphics.hpp>

class DragMouse
{
    sf::Vector2i source_pos;

public:

    DragMouse(sf::Vector2i source_pos);
    virtual void on_move(sf::Vector2i mouse_pos);
    virtual void draw(sf::RenderWindow& window);
};

#endif
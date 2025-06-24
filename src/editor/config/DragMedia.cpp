#include "editor/config/DragMedia.h"

DragMedia::DragMedia(sf::Vector2i source_pos, sf::Color color) :
    DragMouse{source_pos}
{
    icon = sf::RectangleShape(sf::Vector2f(40, 40));
    icon.setFillColor(color);
    icon.setPosition(sf::Vector2f(source_pos));
}

sf::Color DragMedia::get_color()
{
    return icon.getFillColor();
}

void DragMedia::on_move(sf::Vector2i mouse_pos)
{
    icon.setPosition(sf::Vector2f(mouse_pos));
}

void DragMedia::draw(sf::RenderWindow& window)
{
    window.draw(icon);
}
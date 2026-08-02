#include "editor/media/DragMedia.h"

#include "graphics/glRectangle.h"

DragMedia::DragMedia(sf::Color color)
{
    media_color = color;
}

sf::Color DragMedia::get_color()
{
    return media_color;
}

void DragMedia::create_node(GLNode* parent)
{
    GLRectangle* node = GLRectangle::create(parent, sf::Vector2f(), sf::Vector2f(50, 50));
    node->set_fill_color(media_color);
    visible_node = node;
}
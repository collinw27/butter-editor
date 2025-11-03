#include "graphics/RectNode.h"

RectNode::RectNode(Node* parent, sf::Color color, sf::FloatRect rect)
    : SFMLNode(parent)
{
    base_shape = new sf::RectangleShape(rect.size);
    shape = dynamic_cast<sf::RectangleShape*>(base_shape);
    shape->setFillColor(color);
    set_position(rect.position);

    // Must be called here so virtual functions work

    update_all();
}

void RectNode::draw(sf::RenderWindow &window)
{
    window.draw(*shape);
}

void RectNode::set_size(sf::Vector2f size)
{
    shape->setSize(size);
}

void RectNode::set_outline_color(sf::Color color)
{
    shape->setOutlineColor(color);
}

void RectNode::set_outline_thickness(float thickness)
{
    shape->setOutlineThickness(thickness);
}

bool RectNode::contains(sf::Vector2f position)
{
    return shape->getGlobalBounds().contains(position);
}

#ifndef RECT_NODE_H
#define RECT_NODE_H

#include "graphics/SFMLNode.h"

class RectNode : public SFMLNode
{
    sf::RectangleShape *shape;

public:

    RectNode(Node* parent, sf::Color color, sf::FloatRect rect);
    
    virtual void draw(sf::RenderWindow &window) override;

    void set_size(sf::Vector2f size);
    void set_outline_color(sf::Color color);
    void set_outline_thickness(float thickness);
    bool contains(sf::Vector2f position);
};

#endif
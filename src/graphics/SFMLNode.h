#ifndef SFML_NODE_H
#define SFML_NODE_H

#include "graphics/Node.h"

// Abstract class for Nodes that use a sf::Transformable

class SFMLNode : public Node
{
protected:

    sf::Transformable *base_shape;

    virtual void sync_position() override;
    virtual void sync_scale() override;

    // Protected constructor prevents initialization
    // (Since there's no specific shape tied to this class)

    SFMLNode(Node* parent);
};

#endif
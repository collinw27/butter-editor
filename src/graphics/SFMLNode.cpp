#include "graphics/SFMLNode.h"

void SFMLNode::sync_position()
{
    base_shape->setPosition(get_global_position());
}

SFMLNode::SFMLNode(Node* parent)
    : Node(parent)
{
}
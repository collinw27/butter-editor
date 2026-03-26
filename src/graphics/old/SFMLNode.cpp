#include "graphics/old/SFMLNode.h"

void SFMLNode::sync_position()
{
    base_shape->setPosition(g_position);
}

void SFMLNode::sync_scale()
{
    base_shape->setScale(g_scale);
}

SFMLNode::SFMLNode(Node* parent)
    : Node(parent)
{
}
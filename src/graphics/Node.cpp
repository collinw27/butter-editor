#include "graphics/Node.h"

#include <algorithm>

#include "utility/Exceptions.h"

Node::Node(Node* parent)
{
    scale = sf::Vector2f(1, 1);
    g_scale = scale;
    if (parent != nullptr)
    {
        parent->add_child(this);
    }
    update_all();
}

void Node::draw(sf::RenderWindow &window)
{
}

void Node::add_child(Node* child)
{
    if (child->parent != nullptr)
    {
        child->parent->remove_child(child);
    }
    children.push_back(child);
    child->parent = this;
}

void Node::remove_child(Node* child)
{
    if (child->parent != this)
    {
        throw ButterException("Cannot remove child from node that isn't its parent");
    }
    children.erase(std::find(children.begin(), children.end(), child));
    child->parent = nullptr;
}

void Node::free()
{
    if (parent != nullptr)
    {
        parent->remove_child(this);
    }
}

Node *Node::get_parent()
{
    return parent;
}

void Node::update_position()
{
    if (parent != nullptr)
    {
        g_position = parent->g_position + parent->g_scale.componentWiseMul(position);
    }
    else
    {
        g_position = position;
    }
    sync_position();
    for (auto child : children)
    {
        child->update_position();
    }
}

void Node::update_scale()
{
    if (parent != nullptr)
    {
        g_scale = parent->g_scale.componentWiseMul(scale);
    }
    else
    {
        g_scale = scale;
    }
    sync_scale();
    for (auto child : children)
    {
        child->update_position();
        child->update_scale();
    }
}

void Node::update_all()
{
    update_position();
    update_scale();
}

void Node::sync_position()
{
}

void Node::sync_scale()
{
}

sf::Vector2f Node::get_position()
{
    return position;
}

void Node::set_position(sf::Vector2f position)
{
    this->position = position;
    update_position();
}

void Node::set_position_axis(Axis axis, float position)
{
    if (axis == Axis::X)
    {
        this->position.x = position;
    }
    else
    {
        this->position.y = position;
    }
    update_position();
}

sf::Vector2f Node::get_scale()
{
    return scale;
}

void Node::set_scale(sf::Vector2f scale)
{
    this->scale = scale;
    update_scale();
}

void Node::set_scale_axis(Axis axis, float scale)
{
    if (axis == Axis::X)
    {
        this->scale.x = scale;
    }
    else
    {
        this->scale.y = scale;
    }
    update_scale();
}
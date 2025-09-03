#include "graphics/Node.h"

#include <algorithm>

#include "utility/Exceptions.h"

void Node::sync_position()
{
}

void Node::sync_all()
{
    sync_position();
}

Node::Node(Node* parent)
{
    if (parent != nullptr)
    {
        parent->add_child(this);
    }
    sync_position();
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

sf::Vector2f Node::get_position()
{
    return position;
}

sf::Vector2f Node::get_global_position()
{
    if (parent != nullptr)
    {
        return parent->get_position() + position;
    }
    else
    {
        return position;
    }
}

void Node::set_position(sf::Vector2f position)
{
    this->position = position;
    sync_position();
    for (auto child : children)
    {
        child->sync_position();
    }
}

void Node::set_global_position(sf::Vector2f position)
{
    if (parent != nullptr)
    {
        this->position = position - parent->get_position();
    }
    else
    {
        this->position = position;
    }
    sync_position();
    for (auto child : children)
    {
        child->sync_position();
    }
}

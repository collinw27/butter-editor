#ifndef NODE_H
#define NODE_H

#include <vector>
#include <SFML/Graphics.hpp>

enum class Axis
{
    X,
    Y
};

// Nodes are used to group visual elements into a tree structure
// They function very similar to Node2D in the Godot engine
// A Node's transform is applied relative to its parent's
// Modifying a parent's state will apply to the hierarchy as a whole

class Node
{
    Node* parent = nullptr;
    std::vector<Node*> children;

protected:

    sf::Vector2f position;
    sf::Vector2f g_position;
    sf::Vector2f scale;
    sf::Vector2f g_scale;

public:

    Node(Node* parent);

    // Calling `draw()` on a Node will not automatically draw its children
    // It would be possible to do this in theory, but it could easily lead
    // to ambiguous situations where some nodes are drawn multiple times
    // As a rule, whatever object the Node's memory should be responsible
    // for drawing it

    virtual void draw(sf::RenderWindow &window);

    void add_child(Node* child);
    void remove_child(Node* child);
    void free();
    Node* get_parent();

protected:

    void update_position();
    void update_scale();
    void update_all();
    virtual void sync_position();
    virtual void sync_scale();

public:

    virtual sf::Vector2f get_position();
    virtual void set_position(sf::Vector2f position);
    void set_position_axis(Axis axis, float position);
    
    virtual sf::Vector2f get_scale();
    virtual void set_scale(sf::Vector2f scale);
    void set_scale_axis(Axis axis, float scale);
};

#endif
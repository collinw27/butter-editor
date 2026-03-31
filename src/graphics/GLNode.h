#ifndef GLNODE_H
#define GLNODE_H

#include <vector>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>

enum class Axis
{
    X,
    Y
};

// Nodes are used to group visual elements into a tree structure
// They function very similar to Node2D in the Godot engine
// A Node's transform is applied relative to its parent's
// Modifying a parent's state will apply to the hierarchy as a whole

// Unlike pure SFML, `draw()` is not called by the user, but is instead
// delegated to the tree hierarchy to handle
// This allows nodes to have more control over drawing, especially when
// it comes to things like scissor buffering

class GLRootNode;

class GLNode
{
    GLNode* parent = nullptr;
    std::vector<GLNode*> children {};

protected:

    bool is_root_node = false;

    // Position and scale are stored locally
    // Global matrix stores the result of composing local transformations
    // with those of its parent hierarchy

    sf::Vector2f position {0, 0};
    sf::Vector2f scale {1, 1};
    glm::mat4 global_matrix;

protected:

    // Constructor cannot be called publicly
    // `init()` is used for virtual function calls that cannot be used
    // within the constructor
    // `create()`: Auxiliary function for ensuring `init()` is called after constructor

    GLNode(GLNode* parent);
    virtual void init();

public:

    static GLNode* create(GLNode* parent);

protected:

    // Position and scale are fully controlled by the base GLNode class
    // (i.e. subclasses cannot modify the way these are updated)
    // These values can still be used for visuals if desired, but
    // `apply_position()` and `apply_scale()` give the option to set
    // different internal state based on the needs of each specific subclass

    virtual void apply_position();
    virtual void apply_scale();

    virtual void draw();
    virtual void on_window_resized();

public:

    void add_child(GLNode* child);
    void remove_child(GLNode* child);
    void free();
    GLNode* get_parent();

    sf::Vector2f get_position();
    sf::Vector2f get_global_position();
    void set_position(sf::Vector2f position);
    void set_position_axis(Axis axis, float position);
    
    sf::Vector2f get_scale();
    sf::Vector2f get_global_scale();
    void set_scale(sf::Vector2f scale);
    void set_scale_axis(Axis axis, float scale);

    glm::mat4 get_local_matrix();
    glm::mat4 get_global_matrix();
    void update_global_matrix();

    friend GLRootNode;
};

#endif
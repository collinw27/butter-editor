#include "graphics/GLNode.h"

#include <glm/ext/matrix_transform.hpp>
#include <algorithm>
#include "utility/core.h"
#include "utility/Logger.h"

GLNode::GLNode(GLNode* parent)
{
    if (parent != nullptr)
        parent->add_child(this);
}

void GLNode::init()
{
    update_global_matrix();
}

GLNode* GLNode::create(GLNode* parent)
{
    GLNode* instance = new GLNode{parent};
    instance->init();
    return instance;
}

GLNode::~GLNode()
{
    // Free all children from the tree, but DON'T delete them

    for (GLNode* child : children)
        child->orphan();
    orphan();
}

void GLNode::draw()
{
    for (GLNode* child : children)
    {
        if (child->is_visible())
            child->draw();
    }
}

void GLNode::on_window_resized()
{
    for (GLNode* child : children)
        child->on_window_resized();
}

void GLNode::apply_global_matrix() {}

void GLNode::add_child(GLNode* child)
{
    if (child->is_root_node)
        throw ButterException("Root node cannot be added as child");
    if (child->parent != nullptr)
        child->parent->remove_child(child);
    children.push_back(child);
    child->parent = this;
    child->update_global_matrix();
}

void GLNode::remove_child(GLNode* child)
{
    if (child->parent != this)
        throw ButterException("Cannot remove child from node that isn't its parent");
    children.erase(std::find(children.begin(), children.end(), child));
    child->parent = nullptr;
}

void GLNode::orphan()
{
    if (parent != nullptr)
        parent->remove_child(this);
}

void GLNode::reparent(GLNode* new_parent)
{
    orphan();
    if (new_parent != nullptr)
        new_parent->add_child(this);
}

void GLNode::free_children()
{
    for (GLNode* child : children)
        child->orphan();
}

GLNode* GLNode::get_parent()
{
    return parent;
}

const std::vector<GLNode*> GLNode::get_children()
{
    return children;
}

bool GLNode::is_visible()
{
    return visible;
}

void GLNode::set_visible(bool visible)
{
    this->visible = visible;
}


sf::Vector2f GLNode::get_position()
{
    return position;
}

sf::Vector2f GLNode::get_scale()
{
    return scale;
}

// Right now, global transformations take into account the downscaling performed
// by the root node by (window_width, window_height)
// This is technically correct behavior, but doesn't quite match what you would
// expect to be returned
// I'm not currently sure what I'll do about this, but since there's no need for
// these functions right now, I'm just leaving them unimplemented 

/*
// Calculate global value by appling global transformation matrix to a point

sf::Vector2f GLNode::get_global_position()
{
    glm::vec4 translated_point = global_matrix * glm::vec4(0, 0, 0, 1);
    return sf::Vector2f(translated_point.x, -translated_point.y);
}

// Calculate global value by applying global transformation to 2 points,
// and checking the proportion of the space between them

sf::Vector2f GLNode::get_global_scale()
{
    glm::vec4 point_a = global_matrix * glm::vec4(0, 0, 0, 1);
    glm::vec4 point_b = global_matrix * glm::vec4(1, 1, 1, 1);
    return sf::Vector2f(point_b.x - point_a.x, point_b.y - point_a.y);
}
*/

void GLNode::set_position(sf::Vector2f position)
{
    this->position = position;
    update_global_matrix();
    apply_position();
}

void GLNode::set_scale(sf::Vector2f scale)
{
    this->scale = scale;
    apply_scale();
    update_global_matrix();
}

void GLNode::set_position_axis(Axis axis, float position)
{
    if (axis == Axis::X)
        this->position.x = position;
    else
        this->position.y = position;
    update_global_matrix();
    apply_position();
}

void GLNode::set_scale_axis(Axis axis, float scale)
{
    if (axis == Axis::X)
        this->scale.x = scale;
    else
        this->scale.y = scale;
    apply_scale();
    update_global_matrix();
}

glm::mat4 GLNode::get_local_matrix()
{
    // The y-coordinate is negative since OpenGL uses bottom-left as (0, 0),
    // whereas our API uses the more conventional top-left as (0, 0)

    return glm::scale(glm::translate(glm::mat4(1),
        glm::vec3(position.x, -position.y, 0.f)),
        glm::vec3(scale.x, scale.y, 1.f));
}

glm::mat4 GLNode::get_global_matrix()
{
    return global_matrix;
}

void GLNode::update_global_matrix()
{
    if (parent != nullptr)
        global_matrix = parent->get_global_matrix() * get_local_matrix();
    else
        global_matrix = get_local_matrix();
    for (GLNode* child : children)
        child->update_global_matrix();
    apply_global_matrix();
}

void GLNode::apply_position() {}
void GLNode::apply_scale() {}
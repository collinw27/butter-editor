#include "graphics/GLNode.h"

#include <glm/ext/matrix_transform.hpp>
#include <algorithm>
#include "utility/Exceptions.h"

GLNode::GLNode(GLNode* parent)
{
    if (parent != nullptr)
        parent->add_child(this);
    init();
}

void GLNode::init()
{
    update_global_matrix();
    apply_global_matrix();
}

GLNode* GLNode::create(GLNode* parent)
{
    GLNode* instance = new GLNode(parent);
    instance->init();
    return instance;
}

void GLNode::update_global_matrix()
{
    if (parent != nullptr)
        global_matrix = parent->get_global_matrix() * get_local_matrix();
    else
        global_matrix = get_local_matrix();
}

void GLNode::apply_global_matrix() {}

void GLNode::draw() {}

void GLNode::on_window_resized() {}

void GLNode::add_child(GLNode* child)
{
    if (child->is_root_node)
        throw ButterException("Root node cannot be added as child");
    if (child->parent != nullptr)
        child->parent->remove_child(child);
    children.push_back(child);
    child->parent = this;
}

void GLNode::remove_child(GLNode* child)
{
    if (child->parent != this)
        throw ButterException("Cannot remove child from node that isn't its parent");
    children.erase(std::find(children.begin(), children.end(), child));
    child->parent = nullptr;
}

void GLNode::free()
{
    if (parent != nullptr)
        parent->remove_child(this);
}

GLNode* GLNode::get_parent()
{
    return parent;
}

sf::Vector2f GLNode::get_position()
{
    return position;
}

// Calculate global value by appling global transformation matrix to a point

sf::Vector2f GLNode::get_global_position()
{
    glm::vec4 translated_point = glm::vec4(position.x, position.y, 0, 1) * global_matrix;
    return sf::Vector2f(translated_point.x, translated_point.y);
}

void GLNode::set_position(sf::Vector2f position)
{
    this->position = position;
    update_global_matrix();
    apply_global_matrix();
}

void GLNode::set_position_axis(Axis axis, float position)
{
    if (axis == Axis::X)
        this->position.x = position;
    else
        this->position.y = position;
    update_global_matrix();
    apply_global_matrix();
}

sf::Vector2f GLNode::get_scale()
{
    return scale;
}

// Calculate global value by applying global transformation to 2 points,
// and checking the proportion of the space between them

sf::Vector2f GLNode::get_global_scale()
{
    glm::vec4 point_a = glm::vec4(0, 0, 0, 1) * global_matrix;
    glm::vec4 point_b = glm::vec4(1, 1, 1, 1) * global_matrix;
    return sf::Vector2f(point_b.x - point_a.x, point_b.y - point_a.y);
}

void GLNode::set_scale(sf::Vector2f scale)
{
    this->scale = scale;
    update_global_matrix();
    apply_global_matrix();
}

void GLNode::set_scale_axis(Axis axis, float scale)
{
    if (axis == Axis::X)
        this->scale.x = scale;
    else
        this->scale.y = scale;
    update_global_matrix();
    apply_global_matrix();
}

glm::mat4 GLNode::get_local_matrix()
{
    return glm::translate(glm::scale(glm::mat4(1),
        glm::vec3(scale.x, scale.y, 1.f)),
        glm::vec3(position.x, position.y, 0.f));
}

glm::mat4 GLNode::get_global_matrix()
{
    return global_matrix;
}
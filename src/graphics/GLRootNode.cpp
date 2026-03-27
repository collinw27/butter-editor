#include "graphics/GLRootNode.h"

#include "utility/Graphics.h"

GLRootNode::GLRootNode() : GLNode(nullptr)
{
    is_root_node = true;
    sf::Vector2u window_size = Graphics().get_window().getSize();
    scale = sf::Vector2f(1.f / window_size.x, 1.f / window_size.y);
}

GLRootNode* GLRootNode::create()
{
    GLRootNode* instance = new GLRootNode();
    instance->init();
    return instance;
}

void GLRootNode::draw_all()
{
    for (GLNode* child : children)
        child->draw();
}

void GLRootNode::on_window_resized_all()
{
    on_window_resized();
    for (GLNode* child : children)
        child->on_window_resized();
}

// This node will automatically take the inverse scale of the window
// so its children maintain their size when the window is resized
// This is useful for GUI elements, but it can also be overridden by
// scaling another object if undesirable

void GLRootNode::on_window_resized()
{
    sf::Vector2u window_size = Graphics().get_window().getSize();
    set_scale(sf::Vector2f(1.f / window_size.x, 1.f / window_size.y));
}
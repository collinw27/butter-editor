#include "graphics/GLRootNode.h"

GLRootNode::GLRootNode() : GLNode(nullptr)
{
    is_root_node = true;
}

GLRootNode* GLRootNode::create()
{
    GLRootNode* instance = new GLRootNode();
    instance->init();
    return instance;
}

void GLRootNode::draw_all()
{
    for (GLNode* c : children)
        c->draw();
}

void GLRootNode::on_window_resized_all()
{
    for (GLNode* c : children)
        c->on_window_resized();
}
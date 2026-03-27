#ifndef GL_ROOT_NODE_H
#define GL_ROOT_NODE_H

#include "graphics/GLNode.h"

// Similar to any other node, but provides an interface for
// sending events down the node tree
// CANNOT be the child of another node

class GLRootNode : public GLNode
{
    GLRootNode();

public:

    static GLRootNode* create();

    // Public-accessible methods call the protected versions
    // on all children in the node tree

    void on_window_resized_all();
    void draw_all();
};

#endif
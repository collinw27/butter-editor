#ifndef BUTTERGL_WINDOW_NODE_H
#define BUTTERGL_WINDOW_NODE_H

#include "graphics/GLNode.h"

// The window's root node: responsible for propogating draw
// calls to every other node and also responding to window changes
// CANNOT be the child of another node

class GraphicsSingleton;

class GLWindowNode : public GLNode
{
    GLWindowNode();

public:

    static GLWindowNode* create();

protected:

    virtual void on_window_resized() override;

public:

    // Public-accessible methods call the protected versions
    // on all children in the node tree

    friend GraphicsSingleton;
};

#endif
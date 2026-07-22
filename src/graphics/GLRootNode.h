#ifndef BUTTERGL_ROOT_NODE_H
#define BUTTERGL_ROOT_NODE_H

#include "graphics/GLNode.h"

// Similar to any other node, but provides an interface for
// sending events down the node tree
// CANNOT be the child of another node

class GraphicsSingleton;

class GLRootNode : public GLNode
{
    GLRootNode();

public:

    static GLRootNode* create();

protected:

    virtual void on_window_resized() override;

public:

    // Public-accessible methods call the protected versions
    // on all children in the node tree

    friend GraphicsSingleton;
};

#endif
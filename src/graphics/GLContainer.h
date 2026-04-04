#ifndef GL_CONTAINER_H
#define GL_CONTAINER_H

#include "graphics/GLNode.h"

// Node that only displays things within a bounded window
// Anything outside is clipped

class GLContainer : public GLNode
{
    sf::Vector2f size {1, 1};

protected:
    
    GLContainer(GLNode* parent, sf::Vector2f position, sf::Vector2f size);

public:

    static GLContainer* create(GLNode* parent, sf::Vector2f position = {0, 0}, sf::Vector2f size = {1, 1});

    virtual void draw() override;

    void set_size(sf::Vector2f size);
    sf::Vector2f get_size();
};

#endif
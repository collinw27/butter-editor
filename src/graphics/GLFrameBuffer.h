#ifndef BUTTERGL_FRAMEBUFFER_H
#define BUTTERGL_FRAMEBUFFER_H

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "graphics/GLNode.h"
#include "graphics/texture/GLBufferTexture.h"

class GraphicsSingleton;

class GLFrameBuffer : public GLNode
{
    sf::Vector2f size;
    sf::RenderTexture sf_texture;
    GLBufferTexture* gl_texture;
    bool is_active = false;

protected:
    
    GLFrameBuffer(sf::Vector2u buffer_size);

public:

    static GLFrameBuffer* create(sf::Vector2u buffer_size);

protected:
    
    virtual void draw() override;

public:

    const GLBufferTexture* get_texture();
    sf::Vector2f get_size();

    // Public-accessible methods call the protected versions
    // on all children in the node tree

    friend GraphicsSingleton;
};

#endif
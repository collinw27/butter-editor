#ifndef BUTTERGL_FRAMEBUFFER_H
#define BUTTERGL_FRAMEBUFFER_H

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "graphics/GLNode.h"
#include "graphics/texture/GLBufferTexture.h"

class GraphicsSingleton;

// Important note:
// This framebuffer DOES have its own context, but within SFML,
// all contexts share their shareable resources with each other
// (See https://www.sfml-dev.org/tutorials/3.0/window/opengl/#managing-multiple-opengl-windows)

class GLFrameBuffer : public GLNode
{
    sf::Vector2f size;
    sf::RenderTexture sf_texture;
    GLBufferTexture* gl_texture;
    bool is_active = false;
    sf::Color clear_color = sf::Color::Black;

protected:
    
    GLFrameBuffer(sf::Vector2u buffer_size);

public:

    static GLFrameBuffer* create(sf::Vector2u buffer_size);

protected:
    
    virtual void draw() override;

public:

    const GLBufferTexture* get_texture();
    sf::Vector2f get_size();
    void set_clear_color(sf::Color new_color);

    // Public-accessible methods call the protected versions
    // on all children in the node tree

    friend GraphicsSingleton;
};

#endif
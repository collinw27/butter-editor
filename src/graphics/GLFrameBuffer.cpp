#include "graphics/GLFrameBuffer.h"

#include <utility>
#include "utility/core.h"
#include "utility/Graphics.h"

GLFrameBuffer::GLFrameBuffer(sf::Vector2u buffer_size)
    : GLNode{nullptr}, sf_texture{buffer_size}, size{buffer_size}
{
    is_root_node = true;
    gl_texture = new GLBufferTexture(sf_texture.getTexture().getNativeHandle(), buffer_size);

    // Weird position/scale responsible for moving origin to top-left

    position = sf::Vector2f(0, 1.0);
    scale = sf::Vector2f(1.f / size.x, -1.f / size.y);
}

GLFrameBuffer* GLFrameBuffer::create(sf::Vector2u buffer_size)
{
    GLFrameBuffer* instance = new GLFrameBuffer(buffer_size);
    instance->init();
    return instance;
}

void GLFrameBuffer::draw()
{
    GLNode::draw();
}

const GLBufferTexture* GLFrameBuffer::get_texture()
{
    return gl_texture;
}

sf::Vector2f GLFrameBuffer::get_size()
{
    return size;
}
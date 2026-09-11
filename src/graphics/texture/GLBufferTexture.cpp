#include "graphics/texture/GLBufferTexture.h"

GLBufferTexture::GLBufferTexture(GLuint texture_id, sf::Vector2u texture_size)
{
    this->texture_id = texture_id;
    size = texture_size;
}

GLuint GLBufferTexture::get_texture_ID() const
{
    return texture_id;
}

sf::Vector2f GLBufferTexture::get_size() const
{
    return sf::Vector2f(size);
}
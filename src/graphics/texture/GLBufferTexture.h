#ifndef BUTTERGL_BUFFER_TEXTURE_H
#define BUTTERGL_BUFFER_TEXTURE_H

#include "graphics/texture/GLTextureBase.h"

class GLFrameBuffer;

class GLBufferTexture : public GLTextureBase
{
    GLuint texture_id;
    sf::Vector2u size;

protected:

    // Should only be initialized by framebuffer nodes

    GLBufferTexture(GLuint texture_id, sf::Vector2u texture_size);

public:

    GLuint get_texture_ID() const;
    sf::Vector2f get_size() const;

    friend GLFrameBuffer;
};

#endif
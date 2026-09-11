#ifndef BUTTERGL_TEXTURE_BASE_H
#define BUTTERGL_TEXTURE_BASE_H

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

// Shares a lot of functionality with sf::Texture, but it's
// a separate class for two reasons:
// 1) Allows custom GLSprite class to access functionality
// 2) Allows extension in the future if more features are needed
// (Very similar situation to GLFont/GLText)

class GLTextureBase
{
public:

    virtual ~GLTextureBase() {}

    virtual GLuint get_texture_ID() const = 0;
    virtual sf::Vector2f get_size() const = 0;
};

#endif
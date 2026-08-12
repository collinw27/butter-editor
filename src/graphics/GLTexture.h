#ifndef BUTTERGL_TEXTURE_H
#define BUTTERGL_TEXTURE_H

#include <filesystem>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "graphics/GLNode.h"

// Shares a lot of functionality with sf::Texture, but it's
// a separate class for two reasons:
// 1) Allows custom GLSprite class to access functionality
// 2) Allows extension in the future if more features are needed
// (Very similar situation to GLFont/GLText)

class GLTexture
{
    GLuint texture_ID;
    sf::Vector2u size;
    bool do_linear_filter = true;

public:

    GLTexture(std::filesystem::path tex_location);
    ~GLTexture();

    GLuint get_texture_ID();
    sf::Vector2f get_size();

    void set_linear_filter(bool value);
};

#endif
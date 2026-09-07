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
    // `tex_image` doesn't technically need to be persistently stored
    // in order for OpenGL to draw it
    // However, it is still stored here for easy copying and so the
    // image can be returned

    sf::Image tex_image;
    GLuint texture_ID;
    sf::Vector2u size;
    bool do_linear_filter = true;

public:

    GLTexture(std::filesystem::path image_location);
    GLTexture(const sf::Image& source_image);
    GLTexture(const GLTexture& source);

private:
    void finish_setup();

public:

    ~GLTexture();

    GLuint get_texture_ID() const;
    sf::Vector2f get_size() const;

    void set_linear_filter(bool value);
};

#endif
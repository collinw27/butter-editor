#include "graphics/GLTexture.h"

#include "utility/core.h"
#include "utility/Graphics.h"

GLTexture::GLTexture(std::filesystem::path tex_location)
{
    Graphics().window_set_active(true);

    // Load raw texture data
    // SFML used instead of STB since it's readily available

    sf::Image tex_image;
    bool success = tex_image.loadFromFile(tex_location);
    if (!success)
        throw ButterException("Cannot load texture \"" + tex_location.string() + "\"");
    size = tex_image.getSize();

    // Generate texture

    glGenTextures(1, &texture_ID);
    glBindTexture(GL_TEXTURE_2D, texture_ID);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        size.x,
        size.y,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        tex_image.getPixelsPtr()
    );

    // Set texture options
    // Linear is the default filtering since the vast majority of textures will use it
    // Same goes for CLAMP_TO_EDGE, since there is not much use for repeating textures
    // in this program, and keeping the default will cause weirdness when the texture
    // wraps around at an edge

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
    Graphics().window_set_active(false);
}

GLTexture::~GLTexture() {}

GLuint GLTexture::get_texture_ID()
{
    return texture_ID;
}

sf::Vector2f GLTexture::get_size()
{
    return sf::Vector2f(size);
}

void GLTexture::set_linear_filter(bool value)
{
    do_linear_filter = value;
    
    Graphics().window_set_active(true);
    glBindTexture(GL_TEXTURE_2D, texture_ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, do_linear_filter ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, do_linear_filter ? GL_LINEAR : GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    Graphics().window_set_active(false);
}
#include "graphics/GLFont.h"

#include <string>
#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include "utility/Graphics.h"
#include "utility/core.h"

// Created with help from https://learnopengl.com/In-Practice/Text-Rendering

GLFont::GLFont(std::filesystem::path ttf_location)
{
    std::string ttf_string = ttf_location.string();
    const char* ttf_const_char = ttf_string.c_str();
    if (FT_New_Face(Graphics().ft_lib(), ttf_const_char, 0, &font_face))
        throw ButterException("Error loading font " + ttf_location.string());
}

GLFont::~GLFont()
{
    FT_Done_Face(font_face);
}

void GLFont::load_char_map(unsigned int char_size)
{
    Graphics().window_set_active(true);

    // Ensure this size hasn't been cached yet

    if (loaded_chars.find(char_size) != loaded_chars.end())
        return;

    std::map<char, FontChar> char_map;
    FT_Set_Pixel_Sizes(font_face, 0, char_size);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // Load character glyph 

        if (FT_Load_Char(font_face, c, FT_LOAD_RENDER))
            throw ButterException("Could not load glyph '" + std::string(1, c) + "'");

        // Generate texture

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            font_face->glyph->bitmap.width,
            font_face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            font_face->glyph->bitmap.buffer
        );

        // Set texture options

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Now store character for later use

        FontChar font_char = {
            texture, 
            glm::ivec2(font_face->glyph->bitmap.width, font_face->glyph->bitmap.rows),
            glm::ivec2(font_face->glyph->bitmap_left, font_face->glyph->bitmap_top),
            font_face->glyph->advance.x
        };
        char_map.insert({c, font_char});
    }

    // Now that the char map is populated, cache it for this font size

    loaded_chars.insert({char_size, char_map});
    
    Graphics().window_set_active(false);
}

std::map<char, FontChar>& GLFont::get_char_map(unsigned int char_size)
{
    if (loaded_chars.find(char_size) == loaded_chars.end())
        load_char_map(char_size);
    return (loaded_chars.find(char_size)->second);
}
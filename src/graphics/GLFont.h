
// Shares a lot of functionality with sf::Font, but it's
// a separate class for two reasons:
// 1) Allows custom GLText class to access functionality
// 2) Allows extension in the future if more features are needed

// Created with help from https://learnopengl.com/In-Practice/Text-Rendering

#include <filesystem>
#include <map>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

class GLText;

struct FontChar
{
    unsigned texture_ID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    long advance;
};

class GLFont
{
    FT_Face font_face;
    std::map<unsigned, std::map<char, FontChar>> loaded_chars {};

public:

    GLFont(std::filesystem::path ttf_location);
    ~GLFont();
    void load_char_map(unsigned char_size);
    std::map<char, FontChar>& get_char_map(unsigned char_size);
};
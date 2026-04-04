#include "graphics/GLText.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "utility/Exceptions.h"
#include "utility/Graphics.h"
#include "utility/Math.h"
#include "graphics/GLFont.h"

GLText::GLText(GLNode* parent, GLFont* font, unsigned char_size, std::string str)
    : GLNode(parent)
{
    this->font = font;
    if (font == nullptr)
        throw ButterException("Invalid font");

    this->str = str;
    this->char_size = char_size;
    u_color = {1, 1, 1};
}

void GLText::init()
{
    GLNode::init();
    setup_GL();
}

GLText* GLText::create(GLNode* parent, GLFont* font, unsigned char_size, std::string str)
{
    GLText* instance = new GLText(parent, font, char_size, str);
    instance->init();
    return instance;
}

void GLText::on_window_resized()
{
    update_model_matrix();
}

void GLText::draw()
{
    sf::RenderWindow& window = Graphics().get_window();
    glUseProgram(shader_program);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);

    GLuint loc = glGetUniformLocation(shader_program, "model");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(u_model_mat));
    loc = glGetUniformLocation(shader_program, "text_color");
    glUniform3fv(loc, 1, glm::value_ptr(u_color));

    // Since each glyph is its own distinct texture, we need to re-bind
    // the vertex data for every glyph we draw
    // In the future, it would be beneficial to have GLFont stitch all
    // glyphs together into a single sprite sheet
    // (I think sf::Font does this already)

    std::map<char, FontChar>& char_map = font->get_char_map(char_size);
    float x_offset = 0;
    float y_offset = char_map.find('|')->second.size.y;
    for (int i = 0; i < str.length(); ++i)
    {
        // Ignore characters with no associated glyph

        auto ch_it = char_map.find(str.at(i));
        if (ch_it == char_map.end())
            continue;
        FontChar ch = ch_it->second;

        // The height of the '|' character is added by default
        // The characters then "grow upward" based on ch.size.y
        // Note that y coordinates are negative to match the way
        // our provided coordinates start from the top-left, not bottom-left

        float x = x_offset + ch.bearing.x;
        float y = y_offset + ch.size.y - ch.bearing.y;
        GLfloat vertices[] = {
            x, -y + ch.size.y, 0.f, 0.f, 0.f,
            x, -y, 0.f, 0.f, 1.f,
            x + ch.size.x, -y, 0.f, 1.f, 1.f,
            x + ch.size.x, -y + ch.size.y, 0.f, 1.f, 0.f
        };
        x_offset += (ch.advance >> 6);
        
        glBindTexture(GL_TEXTURE_2D, ch.texture_ID);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * 4 * sizeof(GLfloat), vertices);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLNode::draw();
}

void GLText::apply_global_matrix()
{
    update_model_matrix();
}

void GLText::set_string(std::string str)
{
    this->str = str;
    update_model_matrix();
}

std::string GLText::get_string()
{
    return str;
}

unsigned GLText::get_char_size()
{
    return char_size;
}

void GLText::set_char_size(unsigned new_size)
{
    char_size = new_size;
}

sf::Color GLText::get_color()
{
    return text_color;
}

void GLText::set_color(sf::Color color)
{
    text_color = color;
    u_color = to_gl3(text_color);
}

// Returns the offset from within this object
// If index is outside the range of the string, applies to last char

sf::Vector2f GLText::find_char_pos(unsigned index)
{
    // See `draw()` for more info on calculations

    std::map<char, FontChar>& char_map = font->get_char_map(char_size);
    float x_offset = 0;
    const float y_offset = char_map.find('A')->second.size.y;
    float last_y = 0;
    for (int i = 0; i < std::min((unsigned)str.length(), index); ++i)
    {
        auto ch_it = char_map.find(str.at(i));
        if (ch_it == char_map.end())
            continue;
        FontChar ch = ch_it->second;
        float x = x_offset + ch.bearing.x;
        float y = y_offset + ch.size.y - ch.bearing.y;
        last_y = y - ch.size.y;
        x_offset += (ch.advance >> 6);
    }
    return sf::Vector2f(x_offset, last_y);
}

void GLText::setup_GL()
{
    Graphics().window_set_active(true);
    
    shader_program = Graphics().link_shader(BuiltinShader::V_TEX_RECT, BuiltinShader::F_GLYPH);
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &vertex_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    GLuint indices[] = {0, 1, 2, 0, 2, 3};
    glGenBuffers(1, &index_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_VBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindVertexArray(0);

    update_model_matrix();
    
    Graphics().window_set_active(false);
}

void GLText::update_model_matrix()
{
    u_model_mat = Graphics().world_to_view() * global_matrix;
}
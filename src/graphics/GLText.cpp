#include "graphics/GLText.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "utility/Exceptions.h"
#include "utility/Graphics.h"
#include "graphics/GLFont.h"

GLText::GLText(GLNode* parent, GLFont* font, unsigned char_size, std::string str)
    : GLNode(parent)
{
    this->font = font;
    if (font == nullptr)
        throw ButterException("Invalid font");

    this->str = str;
    this->char_size = char_size;
    u_fill_color = glm::vec4(1, 1, 1, 1);
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

    GLuint model_loc = glGetUniformLocation(shader_program, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(u_model_mat));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

void GLText::set_char_size(float new_size)
{
    char_size = new_size;
    update_model_matrix();
}

float GLText::get_char_size()
{
    return char_size;
}

// Returns the offset of the position from this object

sf::Vector2f GLText::find_char_pos(unsigned index)
{
    return sf::Vector2f();
}

void GLText::setup_GL()
{
    Graphics().window_set_active(true);
    
    shader_program = Graphics().link_shader(BuiltinShader::V_TEX_RECT, BuiltinShader::F_TEX_RECT);
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &vertex_VBO);
    glBindVertexArray(0);

    update_rendered_text();

    update_model_matrix();
    
    Graphics().window_set_active(false);
}

void GLText::update_rendered_text()
{
    std::vector<GLfloat> vertex_vec {};
    std::vector<GLuint> index_vec {};
    std::map<char, FontChar>& char_map = font->get_char_map(char_size);
    float current_x = 0;
    for (int i = 0; i < str.length(); ++i)
    {
        auto ch_it = char_map.find(str.at(i));
        FontChar ch = (ch_it == char_map.end()) ? char_map.find('?')->second : ch_it->second;

        float x = current_x + ch.bearing.x;
        float y = -(ch.size.y - ch.bearing.y);
        vertex_vec.insert(vertex_vec.end(), {
            x, y - ch.size.y, 0.f, 0.f, 0.f,
            x, y, 0.f, 0.f, 1.f,
            x + ch.size.x, y, 0.f, 1.f, 1.f,
            x + ch.size.x, y - ch.size.y, 0.f, 1.f, 0.f
        });
        GLuint s = (GLuint)i * 4u;
        index_vec.insert(index_vec.end(), {s + 0, s + 1, s + 2, s + 0, s + 2, s + 3});
        current_x += (ch.advance >> 6);
    }
    // vertex_vec.insert(vertex_vec.end(), {
    //     0.f, 0.f, 0.f, 0.f, 0.f,
    //     0.f, -1.f, 0.f, 0.f, 1.f,
    //     1.f, -1.f, 0.f, 1.f, 1.f,
    //     1.f, 0.f, 0.f, 1.f, 0.f
    // });
    // index_vec.insert(index_vec.end(), {
    //     0, 1, 3, 1, 2, 3
    // });
    GLfloat* vertices = vertex_vec.data();
    GLuint* indices = index_vec.data();

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_vec.size(), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glGenBuffers(1, &index_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_VBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * index_vec.size(), indices, GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void GLText::update_model_matrix()
{
    // Our coordinates range from (0, 0) -> (window_width, window_height),
    // whereas OpenGL coordinate range from (-1, -1) -> (1, 1)
    // Note that this is where the factor of 2 comes from: 1 - (-1)
    // These transformations should be self-explanatory, but note that
    // these are applied in reverse order as per matrix multiplication convention

    size = sf::Vector2f(char_size * 1 * str.length(), char_size * 2);
    u_model_mat = glm::scale(glm::mat4(1), glm::vec3(size.x, size.y, 1.f));
    u_model_mat = Graphics().world_to_view() * global_matrix * u_model_mat;
}
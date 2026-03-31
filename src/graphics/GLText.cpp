#include "graphics/GLText.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "utility/Exceptions.h"
#include "utility/Graphics.h"

GLText::GLText(GLNode* parent, sf::Vector2f position, std::string str)
    : GLNode(parent)
{
    this->position = position;
    this->str = str;
    u_fill_color = glm::vec4(1, 1, 1, 1);
}

void GLText::init()
{
    GLNode::init();
    setup_GL();
}

GLText* GLText::create(GLNode* parent, sf::Vector2f position, std::string str)
{
    GLText* instance = new GLText(parent, position, str);
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
    GLuint fill_color_loc = glGetUniformLocation(shader_program, "fill_color");
    glUniform4fv(fill_color_loc, 1, glm::value_ptr(u_fill_color));

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

    int success;
    GLchar info_log[512];

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    std::string vs_string = Graphics().get_builtin_shader(BuiltinShader::V_RECT);
    const GLchar* vs_str = vs_string.c_str();
    glShaderSource(vertex_shader, 1, &vs_str, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        throw ButterException("OpenGL Error: " + std::string(info_log));
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fs_string = Graphics().get_builtin_shader(BuiltinShader::F_RECT);
    const GLchar* fs_str = fs_string.c_str();
    glShaderSource(fragment_shader, 1, &fs_str, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        throw ButterException("OpenGL Error: " + std::string(info_log));
    }

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glValidateProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        throw ButterException("OpenGL Error: " + std::string(info_log));
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    Graphics().check_gl_errors();

    // The corner is on the origin to make scaling easy
    // Negative y coordinate is used for parity with GLNode position
    
    GLfloat vertices[] = {
        0.f, 0.f, 0.f,
        0.f, -1.f, 0.f,
        1.f, -1.f, 0.f,
        1.f, 0.f, 0.f
    };
    GLuint indices[] = {
        0, 1, 3, 1, 2, 3
    };
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &vertex_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glGenBuffers(1, &index_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_VBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindVertexArray(0);
    
    glUseProgram(shader_program);
    GLuint color_loc = glGetUniformLocation(shader_program, "fill_color");
    glm::vec4 col = glm::vec4(1, 1, 1, 1);
    glUniform4fv(color_loc, 1, glm::value_ptr(col));

    update_model_matrix();
    
    Graphics().window_set_active(false);
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
#include "graphics/glRectangle.h"

#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "utility/Math.h"
#include "utility/Exceptions.h"
#include "utility/Graphics.h"

GLRectangle::GLRectangle(GLNode* parent, sf::Vector2f position, sf::Vector2f size)
    : GLNode(parent)
{
    set_position(position);
    this->size = size;
    setup_GL();
}

GLRectangle* GLRectangle::create(GLNode* parent, sf::Vector2f position, sf::Vector2f size)
{
    GLRectangle* instance = new GLRectangle(parent, position, size);
    instance->init();
    return instance;
}

void GLRectangle::on_window_resized()
{
    set_model_mat();
}

void GLRectangle::draw()
{
    sf::RenderWindow& window = Graphics().get_window();
    std::ignore = window.setActive(true);
    glUseProgram(shader_program);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

sf::Vector2f GLRectangle::get_size()
{
    return size;
}

void GLRectangle::set_size(sf::Vector2f size)
{
    this->size = size;
}

void GLRectangle::set_fill_color(sf::Color color)
{
    glUseProgram(shader_program);
    fill_color = color;
    GLuint color_loc = glGetUniformLocation(shader_program, "fill_color");
    glm::vec4 col = to_gl(fill_color);
    glUniform4fv(color_loc, 1, glm::value_ptr(col));
}

void GLRectangle::set_outline_color(sf::Color color)
{
    outline_color = color;
}

void GLRectangle::set_outline_thickness(float thickness)
{
    outline_thickness = thickness;
}

void GLRectangle::setup_GL()
{
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

    set_model_mat();
    set_fill_color(fill_color);
}

// Must be called when
// a) The bounds of the rectangle are changed
// b) The scaling of the window is changed

void GLRectangle::set_model_mat()
{
    // Our coordinates range from (0, 0) -> (window_width, window_height),
    // whereas OpenGL coordinate range from (-1, -1) -> (1, 1)
    // Note that this is where the factor of 2 comes from: 1 - (-1)
    // These transformations should be self-explanatory, but note that
    // these are applied in reverse order as per matrix multiplication convention

    glUseProgram(shader_program);
    sf::Vector2u window = Graphics().get_window().getSize();
    glm::mat4 model_mat = glm::scale(glm::mat4(1), glm::vec3(size.x, size.y, 1.f));
    // glm::mat4 model_mat = glm::mat4(1);
    model_mat = GLNode::get_window_matrix() * global_matrix * model_mat;
    GLint model_loc = glGetUniformLocation(shader_program, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model_mat));
}
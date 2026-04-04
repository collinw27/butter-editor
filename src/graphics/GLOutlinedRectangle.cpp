#include "graphics/GLOutlinedRectangle.h"

#include <utility>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "utility/Math.h"
#include "utility/Exceptions.h"
#include "utility/Graphics.h"

GLOutlinedRectangle::GLOutlinedRectangle(GLNode* parent, sf::Vector2f position, sf::Vector2f size)
    : GLNode(parent)
{
    this->position = position;
    this->size = size;
    r_position = position;
    r_size = size;
}

// `setup_GL()` called from `init()` to allow running virtual functions
// responsible for making sure internal values are correct

void GLOutlinedRectangle::init()
{
    GLNode::init();
    setup_GL();
}

GLOutlinedRectangle* GLOutlinedRectangle::create(GLNode* parent, sf::Vector2f position, sf::Vector2f size)
{
    GLOutlinedRectangle* instance = new GLOutlinedRectangle(parent, position, size);
    instance->init();
    return instance;
}

void GLOutlinedRectangle::on_window_resized()
{
    update_model_matrix();
}

void GLOutlinedRectangle::apply_position()
{
    r_position = position - sf::Vector2f(1, 1) * std::max(outline_thickness, 0.f);
    update_model_matrix();
}

void GLOutlinedRectangle::apply_scale()
{
    update_model_matrix();
}

void GLOutlinedRectangle::draw()
{
    sf::RenderWindow& window = Graphics().get_window();
    glUseProgram(shader_program);
    glBindVertexArray(VAO);

    GLuint loc = glGetUniformLocation(shader_program, "model");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(u_model_mat));
    loc = glGetUniformLocation(shader_program, "fill_color");
    glUniform4fv(loc, 1, glm::value_ptr(u_fill_color));
    loc = glGetUniformLocation(shader_program, "outline_color");
    glUniform4fv(loc, 1, glm::value_ptr(u_outline_color));
    loc = glGetUniformLocation(shader_program, "outline_width");
    glUniform2fv(loc, 1, glm::value_ptr(u_outline_width));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    GLNode::draw();
}

void GLOutlinedRectangle::apply_global_matrix()
{
    update_model_matrix();
}

sf::Vector2f GLOutlinedRectangle::get_size()
{
    return size;
}

void GLOutlinedRectangle::set_size(sf::Vector2f size)
{
    this->size = size;
    r_size = size + sf::Vector2f(2, 2) * std::max(outline_thickness, 0.f);
}

sf::Color GLOutlinedRectangle::get_fill_color()
{
    return fill_color;
}

void GLOutlinedRectangle::set_fill_color(sf::Color color)
{
    fill_color = color;
    u_fill_color = to_gl(fill_color);
}

sf::Color GLOutlinedRectangle::get_outline_color()
{
    return outline_color;
}

void GLOutlinedRectangle::set_outline_color(sf::Color color)
{
    outline_color = color;
    u_outline_color = to_gl(outline_color);
}

float GLOutlinedRectangle::get_outline_thickness()
{
    return outline_thickness;
}

void GLOutlinedRectangle::set_outline_thickness(float thickness)
{
    outline_thickness = thickness;
    r_position = position - sf::Vector2f(1, 1) * std::max(outline_thickness, 0.f);
    r_size = size + sf::Vector2f(2, 2) * std::max(outline_thickness, 0.f);
    update_model_matrix();
}

void GLOutlinedRectangle::setup_GL()
{
    Graphics().window_set_active(true);

    shader_program = Graphics().link_shader(BuiltinShader::V_RECT_OUTLINED, BuiltinShader::F_RECT_OUTLINED);

    // The corner is on the origin to make scaling easy
    // Negative y coordinate is used for parity with GLNode position
    // Format is (position [3]) (uv [2])
    
    GLfloat vertices[] = {
        0.f, 0.f, 0.f, 0.f, 0.f,
        0.f, -1.f, 0.f, 0.f, 1.f,
        1.f, -1.f, 0.f, 1.f, 1.f,
        1.f, 0.f, 0.f, 1.f, 0.f
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glGenBuffers(1, &index_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_VBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindVertexArray(0);

    update_model_matrix();
    set_fill_color(fill_color);
    
    Graphics().window_set_active(false);
}

void GLOutlinedRectangle::update_model_matrix()
{
    // Similar to GLRectangle, but we must calculate the local
    // matrix ourselves

    u_model_mat = glm::translate(glm::mat4(1), glm::vec3(r_position.x, -r_position.y, 0.f));
    u_model_mat = glm::scale(u_model_mat, glm::vec3(scale.x, scale.y, 1.f));
    u_model_mat = glm::scale(u_model_mat, glm::vec3(r_size.x, r_size.y, 1.f));
    if (get_parent() != nullptr)
        u_model_mat = get_parent()->get_global_matrix() * u_model_mat;
    u_model_mat = Graphics().world_to_view() * u_model_mat;
    
    u_outline_width = glm::vec2(abs(outline_thickness) / r_size.x, abs(outline_thickness) / r_size.y);
}
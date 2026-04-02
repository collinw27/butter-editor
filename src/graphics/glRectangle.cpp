#include "graphics/GLRectangle.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "utility/Math.h"
#include "utility/Exceptions.h"
#include "utility/Graphics.h"

GLRectangle::GLRectangle(GLNode* parent, sf::Vector2f position, sf::Vector2f size)
    : GLNode(parent)
{
    this->position = position;
    this->size = size;
}

// `setup_GL()` called from `init()` to allow running virtual functions
// responsible for making sure internal values are correct

void GLRectangle::init()
{
    GLNode::init();
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
    update_model_matrix();
}

void GLRectangle::draw()
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

void GLRectangle::apply_global_matrix()
{
    update_model_matrix();
}

sf::Vector2f GLRectangle::get_size()
{
    return size;
}

void GLRectangle::set_size(sf::Vector2f size)
{
    this->size = size;
    update_model_matrix();
}

void GLRectangle::set_fill_color(sf::Color color)
{
    fill_color = color;
    u_fill_color = to_gl(fill_color);
}

void GLRectangle::setup_GL()
{
    Graphics().window_set_active(true);

    shader_program = Graphics().link_shader(BuiltinShader::V_RECT, BuiltinShader::F_RECT);

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

    update_model_matrix();
    set_fill_color(fill_color);
    
    Graphics().window_set_active(false);
}

void GLRectangle::update_model_matrix()
{
    // Our coordinates range from (0, 0) -> (window_width, window_height),
    // whereas OpenGL coordinate range from (-1, -1) -> (1, 1)
    // Note that this is where the factor of 2 comes from: 1 - (-1)
    // These transformations should be self-explanatory, but note that
    // these are applied in reverse order as per matrix multiplication convention

    u_model_mat = glm::scale(glm::mat4(1), glm::vec3(size.x, size.y, 1.f));
    u_model_mat = Graphics().world_to_view() * global_matrix * u_model_mat;
}
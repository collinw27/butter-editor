#include "graphics/GLShaderRectangle.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "utility/core.h"
#include "utility/Graphics.h"

GLShaderRectangle::GLShaderRectangle(GLNode* parent, GLShaderProgram* program, sf::Vector2f position, sf::Vector2f size)
    : GLNode{parent}
{
    this->position = position;
    this->size = size;
    shader_program = std::unique_ptr<GLShaderProgram>(program);
}

// `setup_GL()` called from `init()` to allow running virtual functions
// responsible for making sure internal values are correct

void GLShaderRectangle::init()
{
    GLNode::init();
    setup_GL();
}

GLShaderRectangle* GLShaderRectangle::create(GLNode* parent, GLShaderProgram* program, sf::Vector2f position, sf::Vector2f size)
{
    GLShaderRectangle* instance = new GLShaderRectangle(parent, program, position, size);
    instance->init();
    return instance;
}

void GLShaderRectangle::on_window_resized()
{
    update_model_matrix();
}

void GLShaderRectangle::draw()
{
    sf::RenderWindow& window = Graphics().get_window();
    glBindVertexArray(VAO);

    shader_program->prepare_shader();

    GLuint model_loc = glGetUniformLocation(shader_program->shader_program, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(u_model_mat));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    shader_program->after_shader();

    GLNode::draw();
}

void GLShaderRectangle::apply_global_matrix()
{
    update_model_matrix();
}

sf::Vector2f GLShaderRectangle::get_size()
{
    return size;
}

void GLShaderRectangle::set_size(sf::Vector2f size)
{
    this->size = size;
    update_model_matrix();
}

GLShaderProgram* GLShaderRectangle::get_shader()
{
    return shader_program.get();
}

void GLShaderRectangle::setup_GL()
{
    Graphics().window_set_active(true);

    // The corner is on the origin to make scaling easy
    // Negative y coordinate is used for parity with GLNode position
    
    GLfloat vertices[] = {
        0.f, 0.f, 0.f, 0.f, 0.f,
        0.f, -1.f, 0.f, 0.f, 1.f,
        1.f, -1.f, 0.f, 1.f, 1.f,
        1.f, 0.f, 0.f, 1.f, 0.f
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

    GLuint indices[] = {0, 1, 2, 0, 2, 3};
    glGenBuffers(1, &index_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_VBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindVertexArray(0);

    update_model_matrix();
    
    Graphics().window_set_active(false);
}

void GLShaderRectangle::update_model_matrix()
{
    // Our coordinates range from (0, 0) -> (window_width, window_height),
    // whereas OpenGL coordinate range from (-1, -1) -> (1, 1)
    // Note that this is where the factor of 2 comes from: 1 - (-1)
    // These transformations should be self-explanatory, but note that
    // these are applied in reverse order as per matrix multiplication convention

    u_model_mat = glm::scale(glm::mat4(1), glm::vec3(size.x, size.y, 1.f));
    u_model_mat = Graphics().world_to_view() * global_matrix * u_model_mat;
}
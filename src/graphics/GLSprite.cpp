#include "graphics/GLSprite.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "utility/Graphics.h"

GLSprite::GLSprite(GLNode* parent, GLTexture* texture, sf::Vector2f position)
    : GLNode{parent}
{
    this->texture = texture;
    this->position = position;
    size = this->texture->get_size();
}

void GLSprite::init()
{
    GLNode::init();
    setup_GL();
}

GLSprite* GLSprite::create(GLNode* parent, GLTexture* texture, sf::Vector2f position)
{
    GLSprite* instance = new GLSprite(parent, texture, position);
    instance->init();
    return instance;
}

void GLSprite::on_window_resized()
{
    update_model_matrix();
}

void GLSprite::draw()
{
    sf::RenderWindow& window = Graphics().get_window();
    glUseProgram(shader_program);
    glBindVertexArray(VAO);

    // `tex` might not be default initialized to 0 on all systems
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->get_texture_ID());
    glUniform1i(glGetUniformLocation(shader_program, "tex"), 0);

    GLuint model_loc = glGetUniformLocation(shader_program, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(u_model_mat));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLNode::draw();
}

void GLSprite::apply_global_matrix()
{
    update_model_matrix();
}

sf::Vector2f GLSprite::get_size()
{
    return sf::Vector2f(1, 1);
}

void GLSprite::setup_GL()
{
    Graphics().window_set_active(true);

    shader_program = Graphics().link_shader(BuiltinShader::V_TEX_RECT, BuiltinShader::F_TEX_RECT);
    
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

void GLSprite::update_model_matrix()
{
    // See GLRectangle

    u_model_mat = glm::scale(glm::mat4(1), glm::vec3(size.x, size.y, 1.f));
    u_model_mat = Graphics().world_to_view() * global_matrix * u_model_mat;
}
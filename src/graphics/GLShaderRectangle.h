#ifndef BUTTERGL_SHADER_RECTANGLE_H
#define BUTTERGL_SHADER_RECTANGLE_H

#include <map>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "graphics/GLNode.h"
#include "graphics/shader/GLShaderProgram.h"

class GLShaderRectangle : public GLNode
{
    sf::Vector2f size {1, 1};

    std::unique_ptr<GLShaderProgram> shader_program;

    GLuint VAO;
    GLuint vertex_VBO;
    GLuint index_VBO;

    glm::mat4 u_model_mat;

protected:
    
    GLShaderRectangle(GLNode* parent, GLShaderProgram* program, sf::Vector2f position, sf::Vector2f size);
    virtual void init() override;

public:

    static GLShaderRectangle* create(GLNode* parent, GLShaderProgram* program, sf::Vector2f position = {0, 0}, sf::Vector2f size = {1, 1});

protected:

    virtual void on_window_resized() override;
    virtual void draw() override;
    virtual void apply_global_matrix() override;

public:

    sf::Vector2f get_size();
    void set_size(sf::Vector2f size);
    GLShaderProgram* get_shader();

private:

    void setup_GL();
    void update_model_matrix();
};

#endif
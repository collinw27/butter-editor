#ifndef GL_RECTANGLE_H
#define GL_RECTANGLE_H

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "graphics/GLNode.h"

class GLRectangle : public GLNode
{
    sf::Vector2f size {1, 1};
    sf::Color fill_color {sf::Color::White};

    // For now, each rectangle has its own OpenGL shader program
    // Resources could be saved by minimizing calls to glUseProgram(), although
    // this would need something like a custom batching algorithm to work properly
    // This wouldn't feasible or helpful except in specific circumstances

    GLuint shader_program;
    GLuint VAO;
    GLuint vertex_VBO;
    GLuint index_VBO;

    glm::mat4 u_model_mat;
    glm::vec4 u_fill_color;

protected:
    
    GLRectangle(GLNode* parent, sf::Vector2f position, sf::Vector2f size);
    virtual void init() override;

public:

    static GLRectangle* create(GLNode* parent, sf::Vector2f position = {0, 0}, sf::Vector2f size = {1, 1});

protected:

    virtual void on_window_resized() override;
    virtual void draw() override;
    virtual void apply_global_matrix() override;

public:

    sf::Vector2f get_size();
    void set_size(sf::Vector2f size);

    sf::Color get_fill_color();
    void set_fill_color(sf::Color color);

private:

    void setup_GL();
    void update_model_matrix();
};

#endif
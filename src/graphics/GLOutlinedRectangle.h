#ifndef GL_OUTLINED_RECTANGLE_H
#define GL_OUTLINED_RECTANGLE_H

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "graphics/GLNode.h"

class GLOutlinedRectangle : public GLNode
{
    // Variables prefaced with r_ are used for drawing the rectangle,
    // and don't necessarily sync with the rest of its internal state

    sf::Vector2f size {1, 1};
    sf::Vector2f r_position;
    sf::Vector2f r_size;
    sf::Color fill_color {sf::Color::White};
    sf::Color outline_color {sf::Color::White};
    float outline_thickness = 0.f;

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
    glm::vec4 u_outline_color;
    glm::vec2 u_outline_width;

protected:
    
    GLOutlinedRectangle(GLNode* parent, sf::Vector2f position, sf::Vector2f size);
    virtual void init() override;

public:

    static GLOutlinedRectangle* create(GLNode* parent, sf::Vector2f position = {0, 0}, sf::Vector2f size = {1, 1});

protected:

    virtual void apply_position() override;
    virtual void apply_scale() override;
    virtual void on_window_resized() override;
    virtual void draw() override;
    virtual void apply_global_matrix() override;

public:

    sf::Vector2f get_size();
    void set_size(sf::Vector2f size);

    void set_fill_color(sf::Color color);
    void set_outline_color(sf::Color color);
    void set_outline_thickness(float thickness);

private:

    void setup_GL();
    void update_model_matrix();
};

#endif
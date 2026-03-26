#ifndef GL_RECTANGLE_H
#define GL_RECTANGLE_H

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

class GLRectangle
{
    sf::Vector2f position {0, 0};
    sf::Vector2f size {1, 1};
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

public:

    GLRectangle();
    GLRectangle(sf::Vector2f size);
    GLRectangle(sf::Vector2f position, sf::Vector2f size);

    sf::Vector2f get_position();
    void set_position(sf::Vector2f position);
    sf::Vector2f get_size();
    void set_size(sf::Vector2f size);
    
    void on_window_resized();

    void set_fill_color(sf::Color color);
    void set_outline_color(sf::Color color);
    void set_outline_thickness(float thickness);

    void draw();

private:

    void setup_GL();
    void set_model_mat();
};

#endif
#ifndef GL_TEXT_H
#define GL_TEXT_H

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "graphics/GLNode.h"

// Created with help from https://learnopengl.com/In-Practice/Text-Rendering

class GLFont;

class GLText : public GLNode
{
    sf::Vector2f size {1, 1};

    GLFont* font;
    std::string str;
    unsigned char_size;

    GLuint shader_program;
    GLuint VAO;
    GLuint vertex_VBO;
    GLuint index_VBO;
    
    glm::mat4 u_model_mat;
    glm::vec4 u_fill_color;

protected:
    
    GLText(GLNode* parent, GLFont* font, unsigned char_size, std::string str);
    virtual void init();

public:

    static GLText* create(GLNode* parent, GLFont* font, unsigned char_size, std::string str = "");

protected:

    virtual void on_window_resized() override;
    virtual void draw() override;
    virtual void apply_global_matrix() override;

public:

    void set_string(std::string str);
    std::string get_string();

    void set_char_size(float new_size);
    float get_char_size();

    sf::Vector2f find_char_pos(unsigned index);

private: 

    void setup_GL();
    void update_rendered_text();
    void update_model_matrix();
};

#endif
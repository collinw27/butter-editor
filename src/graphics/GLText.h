#ifndef GL_TEXT_H
#define GL_TEXT_H

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "graphics/GLNode.h"

// For now, much of this class's functionality is a placeholder
// to allow testing OpenGL compatibility in place of SFML without
// having to implement all the nuances of text rendering

class GLText : public GLNode
{
    sf::Vector2f size {1, 1};

    std::string str;
    float char_size = 10;

    GLuint shader_program;
    GLuint VAO;
    GLuint vertex_VBO;
    GLuint index_VBO;
    
    glm::mat4 u_model_mat;
    glm::vec4 u_fill_color;

protected:
    
    GLText(GLNode* parent, sf::Vector2f position, std::string str);
    virtual void init();

public:

    static GLText* create(GLNode* parent, sf::Vector2f position, std::string str);

protected:

    virtual void on_window_resized() override;
    virtual void draw() override;

public:

    void set_string(std::string str);
    std::string get_string();

    void set_char_size(float new_size);
    float get_char_size();

    sf::Vector2f find_char_pos();

private: 

    void setup_GL();
    void set_model_mat();
};

#endif
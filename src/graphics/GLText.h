#ifndef BUTTERGL_TEXT_H
#define BUTTERGL_TEXT_H

#include <map>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "graphics/GLNode.h"
#include "graphics/text_formatting/TextFormat.h"

// Created with help from https://learnopengl.com/In-Practice/Text-Rendering

class GLFont;

class GLText : public GLNode
{
    GLFont* font;
    std::string str;
    unsigned int char_size;
    float line_spacing = 4.f;
    sf::Color text_color {sf::Color::White};

    bool do_special_formatting = false;
    std::map<unsigned int, TextFormat::Instruction*> formatting_body {};

    GLuint shader_program;
    GLuint VAO;
    GLuint vertex_VBO;
    GLuint index_VBO;
    
    glm::mat4 u_model_mat;
    glm::vec3 u_color;

protected:
    
    GLText(GLNode* parent, GLFont* font, unsigned int char_size, std::string str);
    virtual void init();
    
public:
    
    ~GLText();
    static GLText* create(GLNode* parent, GLFont* font);
    static GLText* create(GLNode* parent, GLFont* font, unsigned int char_size, std::string str);

protected:

    virtual void on_window_resized() override;
    virtual void draw() override;
    virtual void apply_global_matrix() override;

public:

    void set_string(std::string str);
    std::string get_string();

    void enable_special_formatting();
    void reset_formatting();
    void add_string(std::string text);
    void add_color(sf::Color color);
    void add_color(sf::Color color, unsigned int position);

    unsigned int get_char_size();
    void set_char_size(unsigned int new_size);
    float get_line_spacing();
    void set_line_spacing(float new_spacing);
    
    sf::Color get_color();
    void set_color(sf::Color color);

    sf::Vector2f find_char_pos(unsigned int index);

private: 

    void setup_GL();
    void update_model_matrix();
};

#endif
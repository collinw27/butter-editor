#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>
#include <vector>
#include <stack>

#include <ft2build.h>
#include FT_FREETYPE_H
#include "gl/glew.h"
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>

constexpr int WINDOW_W = 1280;
constexpr int WINDOW_H = 720;

// This MUST be initialized after FileManager

class GraphicsSingleton;
class GLRootNode;
class GLFont;

GraphicsSingleton& Graphics();

// Vertex prefaced with v
// Fragment prefaced with f

enum class BuiltinShader
{
    V_RECT,
    V_RECT_OUTLINED,
    V_TEX_RECT,
    F_RECT,
    F_RECT_OUTLINED,
    F_TEX_RECT
};

class GraphicsSingleton
{
    static GraphicsSingleton* singleton_object;

    sf::RenderWindow* window = nullptr;
    bool window_is_active = false;
    std::vector<std::string> builtin_shaders {};
    std::stack<sf::IntRect> scissors {};

    GLFont* main_font_obj;
    GLFont* mono_font_obj;

    glm::mat4 world_to_view_matrix;
    glm::mat4 world_to_screen_matrix;

    FT_Library ft_library;

public:

    GraphicsSingleton();
    ~GraphicsSingleton();

    void init(sf::VideoMode mode, std::string title, uint32_t style);
    void display(GLRootNode* root);

    sf::RenderWindow& get_window();
    void window_set_active(bool active);
    void on_window_resized(GLRootNode* root);
    void set_clear_color(sf::Color color);
    
    GLFont* main_font();
    GLFont* mono_font();

    std::string get_builtin_shader(BuiltinShader shader_id);
    GLuint link_shader(BuiltinShader vertex_shader, BuiltinShader fragment_shader);

    glm::mat4 world_to_view();
    glm::mat4 world_to_screen();
    sf::Vector2f screen_to_world(sf::Vector2f vec);

    void check_gl_errors();
    FT_Library& ft_lib();

    unsigned push_scissor(sf::IntRect bounds);
    void pop_scissor(unsigned check_index);


    friend GraphicsSingleton& Graphics();

private:

    void load_builtin_shader(BuiltinShader shader_id, std::string filename);
};

#endif
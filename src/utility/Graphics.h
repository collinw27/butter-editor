#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>
#include <vector>
#include <stack>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>

constexpr int WINDOW_W = 1280;
constexpr int WINDOW_H = 720;

// This MUST be initialized after FileManager

class GraphicsSingleton;
class GLRootNode;

GraphicsSingleton& Graphics();

// Vertex prefaced with v
// Fragment prefaced with f

enum class BuiltinShader
{
    V_RECT,
    F_RECT
};

class GraphicsSingleton
{
    static GraphicsSingleton* singleton_object;

    sf::RenderWindow* window = nullptr;
    bool window_is_active = false;
    std::vector<std::string> builtin_shaders {};
    std::stack<sf::IntRect> scissors {};

    glm::mat4 world_to_view_matrix;
    glm::mat4 world_to_screen_matrix;

public:

    GraphicsSingleton();
    ~GraphicsSingleton();

    void init(sf::VideoMode mode, std::string title, uint32_t style);
    void display(GLRootNode* root);

    sf::RenderWindow& get_window();
    void window_set_active(bool active);
    void on_window_resized(GLRootNode* root);

    std::string get_builtin_shader(BuiltinShader shader_id);
    glm::mat4 world_to_view();
    glm::mat4 world_to_screen();
    sf::Vector2f screen_to_world(sf::Vector2f vec);

    void check_gl_errors();

    unsigned push_scissor(sf::IntRect bounds);
    void pop_scissor(unsigned check_index);

    friend GraphicsSingleton& Graphics();

private:

    void load_builtin_shader(BuiltinShader shader_id, std::string filename);
};

#endif
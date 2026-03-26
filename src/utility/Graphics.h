#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

constexpr int WINDOW_W = 1280;
constexpr int WINDOW_H = 720;

// This MUST be initialized after FileManager

class GraphicsSingleton;

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
    std::vector<std::string> builtin_shaders {};

public:

    GraphicsSingleton();
    ~GraphicsSingleton();

    void create_window(sf::VideoMode mode, std::string title, uint32_t style);
    sf::RenderWindow& get_window();
    std::string get_builtin_shader(BuiltinShader shader_id);

    void check_gl_errors();
    bool clear_gl_errors();

    friend GraphicsSingleton& Graphics();

private:

    void load_builtin_shader(BuiltinShader shader_id, std::string filename);
};

#endif
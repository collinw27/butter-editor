#include "Graphics.h"

#include <utility>

#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "utility/FileManager.h"
#include "utility/Exceptions.h"
#include "graphics/GLRootNode.h"
#include "graphics/GLFont.h"
#include "utility/Math.h"

GraphicsSingleton* GraphicsSingleton::singleton_object = nullptr;

GraphicsSingleton& Graphics()
{
    return *GraphicsSingleton::singleton_object;
}

GraphicsSingleton::GraphicsSingleton()
{
    if (GraphicsSingleton::singleton_object != nullptr)
        throw ButterException("Reinitialization of singleton");
    GraphicsSingleton::singleton_object = this;

    // Basic shaders are loaded automatically for easy access

    load_builtin_shader(BuiltinShader::V_RECT, "rect.vs");
    load_builtin_shader(BuiltinShader::V_RECT_OUTLINED, "rect_outlined.vs");
    load_builtin_shader(BuiltinShader::V_TEX_RECT, "tex_rect.vs");
    load_builtin_shader(BuiltinShader::F_RECT, "rect.fs");
    load_builtin_shader(BuiltinShader::F_RECT_OUTLINED, "rect_outlined.fs");
    load_builtin_shader(BuiltinShader::F_TEX_RECT, "tex_rect.fs");
    load_builtin_shader(BuiltinShader::F_GLYPH, "glyph.fs");

    // Pre-computer matrix values
    // Some will need to be updated later on (ex. on window resize)
    
    world_to_view_matrix = glm::scale(glm::translate(glm::mat4(1), glm::vec3(-1, 1, 0)), glm::vec3(2, 2, 1));
    world_to_screen_matrix = glm::mat4(1);
}

GraphicsSingleton::~GraphicsSingleton()
{
    GraphicsSingleton::singleton_object = nullptr;
}

void GraphicsSingleton::init(sf::VideoMode mode, std::string title, uint32_t style)
{
    if (window != nullptr)
        throw ButterException("Cannot create second window");
    window = new sf::RenderWindow(mode, title, style);
    
    glViewport(0, 0, WINDOW_W, WINDOW_H);
    on_window_resized(nullptr);
    
    if (glewInit() != GLEW_OK)
        throw ButterException("Cannot initialize GLEW");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (FT_Init_FreeType(&ft_library))
        throw ButterException("Cannot initialize FreeType");

    // Load fonts now that initialization is finished

    main_font_obj = new GLFont(FileManager().get_res_path("font/Lato-Bold.ttf"));
    mono_font_obj = new GLFont(FileManager().get_res_path("font/Consolas.ttf"));
}

void GraphicsSingleton::display(GLRootNode* root)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, window->getSize().x, window->getSize().y);
    Graphics().window_set_active(true);
    if (root != nullptr && root->is_visible())
        root->draw();
    glBindVertexArray(0);
    glUseProgram(0);
    Graphics().window_set_active(false);
    window->display();
}

void GraphicsSingleton::on_window_resized(GLRootNode* root)
{
    world_to_screen_matrix = glm::scale(glm::mat4(1), glm::vec3(window->getSize().x, window->getSize().y, 1.f));
    if (root != nullptr)
        root->on_window_resized();
}

sf::RenderWindow& GraphicsSingleton::get_window()
{
    return *window;
}

void GraphicsSingleton::window_set_active(bool active)
{
    if (window_is_active != active)
    {
        window_is_active = active;
        std::ignore = window->setActive(active);
    }
}

void GraphicsSingleton::set_clear_color(sf::Color color)
{
    glClearColor(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
}

GLFont* GraphicsSingleton::main_font()
{
    return main_font_obj;
}

GLFont* GraphicsSingleton::mono_font()
{
    return mono_font_obj;
}

std::string GraphicsSingleton::get_builtin_shader(BuiltinShader shader_id)
{
    if ((int)shader_id >= builtin_shaders.size())
        throw ButterException("Invalid shader request");
    return builtin_shaders.at((int)shader_id);
}

GLuint GraphicsSingleton::link_shader(BuiltinShader vertex_id, BuiltinShader fragment_id)
{
    int success;
    GLchar info_log[512];

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    std::string vs_string = Graphics().get_builtin_shader(vertex_id);
    const GLchar* vs_str = vs_string.c_str();
    glShaderSource(vertex_shader, 1, &vs_str, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        throw ButterException("OpenGL Error: " + std::string(info_log));
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fs_string = Graphics().get_builtin_shader(fragment_id);
    const GLchar* fs_str = fs_string.c_str();
    glShaderSource(fragment_shader, 1, &fs_str, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        throw ButterException("OpenGL Error: " + std::string(info_log));
    }

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glValidateProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        throw ButterException("OpenGL Error: " + std::string(info_log));
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    Graphics().check_gl_errors();

    return shader_program;
}

// For simplicity, the only visible region of world space is (0, 0) to (1, 1)
// This matrix transforms this range to fill the entire clip space

glm::mat4 GraphicsSingleton::world_to_view()
{
    return world_to_view_matrix;
}

glm::mat4 GraphicsSingleton::world_to_screen()
{
    return world_to_screen_matrix;
}

// Most node coordinates are provided in screen coordinates
// This function transforms screen coordinates in the range (0, 0) to WINDOW_SIZE
// to map to the region (0, 0) to (1, 1) in world space
// This is provided as sf::Vector instead of a matrix since 99% of the time,
// screen space coordiantes are provided in sf::Vector format

sf::Vector2f GraphicsSingleton::screen_to_world(sf::Vector2f vec)
{
    return vec.componentWiseMul(sf::Vector2f(1.f / window->getSize().x, 1.f / window->getSize().y));
}

void GraphicsSingleton::check_gl_errors()
{
    GLenum error_code = glGetError();
    if (error_code != GL_NO_ERROR)
    {
        // Thanks to https://learnopengl.com/In-Practice/Debugging

        std::string error = "Unknown";
        switch (error_code)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        throw ButterException("OpenGL error: " + error);
    }
}

FT_Library& GraphicsSingleton::ft_lib()
{
    return ft_library;
}

// Instead of manually setting the glScissor parameters, the states
// are pushed to and popped from a stack
// This ensures that if one is interrupted by another, the proper state
// is restored after the latter is finished
// `push_scissor()` returns a check value that should be passed back to
// `push_scissor_world()` is the same, but converts world to screen coordinates
// `pop_scissor()` ensure the same state is being popped as was pushed

unsigned GraphicsSingleton::push_scissor(sf::IntRect bounds)
{
    scissors.push(bounds);
    glEnable(GL_SCISSOR_TEST);
    glScissor(bounds.position.x, bounds.position.y, bounds.size.x, bounds.size.y);
    return scissors.size();
}

void GraphicsSingleton::pop_scissor(unsigned check_index)
{
    if (scissors.size() != check_index)
        throw ButterException("Mismatched scissor stack index");
    scissors.pop();
    
    if (scissors.empty())
    {
        glDisable(GL_SCISSOR_TEST);
    }
    else
    {
        sf::IntRect bounds = scissors.top();
        glScissor(bounds.position.x, bounds.position.y, bounds.size.x, bounds.size.y);
    }
}

// Shader ID is not needed,
// but is passed to make sure the index is correct

void GraphicsSingleton::load_builtin_shader(BuiltinShader shader_id, std::string filename)
{
    if (builtin_shaders.size() != (int)shader_id)
        throw ButterException("Mismatched shader ID during load");
    builtin_shaders.push_back(FileManager().load_shader(filename));
}
#include "Graphics.h"

#include "gl/glew.h"
#include <SFML/OpenGL.hpp>
#include "utility/FileManager.h"
#include "utility/Exceptions.h"

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
    load_builtin_shader(BuiltinShader::F_RECT, "rect.fs");
}

GraphicsSingleton::~GraphicsSingleton()
{
    GraphicsSingleton::singleton_object = nullptr;
}

void GraphicsSingleton::create_window(sf::VideoMode mode, std::string title, uint32_t style)
{
    if (window != nullptr)
        throw ButterException("Cannot create second window");
    window = new sf::RenderWindow(mode, title, style);
    
    glViewport(0, 0, WINDOW_W, WINDOW_H);
}

sf::RenderWindow& GraphicsSingleton::get_window()
{
    return *window;
}

std::string GraphicsSingleton::get_builtin_shader(BuiltinShader shader_id)
{
    if ((int)shader_id >= builtin_shaders.size())
        throw ButterException("Invalid shader request");
    return builtin_shaders.at((int)shader_id);
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

bool GraphicsSingleton::clear_gl_errors()
{
    GLenum error_code = glGetError();
    if (error_code == GL_NO_ERROR)
        return false;
    // else
    //     while (error_code != GL_NO_ERROR)
    //     {
    //         error_code = glGetError();
    //     }
    return true;
}

// Shader ID is not needed, but is passed in to make sure 
// the index is correct

void GraphicsSingleton::load_builtin_shader(BuiltinShader shader_id, std::string filename)
{
    if (builtin_shaders.size() != (int)shader_id)
        throw ButterException("Mismatched shader ID during load");
    builtin_shaders.push_back(FileManager().load_shader(filename));
}
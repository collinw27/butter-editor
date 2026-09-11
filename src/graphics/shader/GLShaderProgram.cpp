#include "graphics/shader/GLShaderProgram.h"

GLShaderProgram::GLShaderProgram(BuiltinShader vertex_shader, BuiltinShader fragment_shader)
    : shader_program{link_shader(vertex_shader, fragment_shader)}
{
}

GLuint GLShaderProgram::link_shader(BuiltinShader vertex_shader, BuiltinShader fragment_shader)
{
    GLuint output = Graphics().link_shader(vertex_shader, fragment_shader);
    return output;
}
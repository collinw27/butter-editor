#ifndef BUTTERGL_SHADER_PROGRAM_H
#define BUTTERGL_SHADER_PROGRAM_H

#include "utility/Graphics.h"

class GLShaderProgram
{
protected:

    GLShaderProgram(BuiltinShader vertex_shader, BuiltinShader fragment_shader);

public:

    const GLuint shader_program;

    virtual void prepare_shader() = 0;
    virtual void after_shader() = 0;

private:

    GLuint link_shader(BuiltinShader vertex_shader, BuiltinShader fragment_shader);
};

#endif
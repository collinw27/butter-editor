#include "editor/timeline/graphics/ClipThumbnailShader.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utility/Graphics.h"

ClipThumbnailShader::ClipThumbnailShader()
    : GLShaderProgram{BuiltinShader::V_TEX_RECT, BuiltinShader::F_CLIP_THUMBNAIL}
{}

void ClipThumbnailShader::prepare_shader()
{
    glUseProgram(shader_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->get_texture_ID());
    GLuint loc = glGetUniformLocation(shader_program, "x_shrink");
    glUniform1f(loc, x_shrink);
    loc = glGetUniformLocation(shader_program, "tex");
    glUniform1i(loc, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void ClipThumbnailShader::after_shader()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
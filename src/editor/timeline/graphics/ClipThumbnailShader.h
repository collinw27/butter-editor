#ifndef CLIP_THUMBNAIL_SHADER_H
#define CLIP_THUMBNAIL_SHADER_H

#include <memory>
#include <SFML/Graphics.hpp>
#include "graphics/GLTexture.h"
#include "graphics/shader/GLShaderProgram.h"

class ClipThumbnailShader : public GLShaderProgram
{
public:

    GLfloat x_shrink = 1.0;
    const GLTexture* texture = nullptr;

    ClipThumbnailShader();
    virtual void prepare_shader() override;
    virtual void after_shader() override;
};

#endif
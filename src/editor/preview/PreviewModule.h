#ifndef PREVIEW_MODULE_H
#define PREVIEW_MODULE_H

#include "editor/core/EditorModule.h"
#include "graphics/nodes.h"
#include "graphics/texture/GLBufferTexture.h"

class PreviewModule : public EditorModule
{
    const GLBufferTexture* video_texture = nullptr;
    std::unique_ptr<GLSprite> video_output;

public:

    PreviewModule(Editor& editor);
    void set_video_output(const GLBufferTexture* buffer);
    
    virtual void apply_bounds() override;

private:

    void fit_video_output();
};

#endif
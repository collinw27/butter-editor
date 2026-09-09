#ifndef DRAG_MEDIA_H
#define DRAG_MEDIA_H

#include <memory>
#include "graphics/nodes.h"
#include "utility/core.h"
#include "editor/core/mouse/DragMouseEvent.h"

class DragMedia : public DragMouseEvent
{
    const GLTexture* thumbnail_tex;

public:

    const id_s media_id;
    VideoTime start_time = 0;
    VideoTime length = 0;
    bool valid = false;

    DragMedia(id_s media_id, GLTexture* thumbnail_tex);

    virtual void create_node(GLNode* parent) override;
};

#endif
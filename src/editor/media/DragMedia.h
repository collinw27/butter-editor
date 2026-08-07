#ifndef DRAG_MEDIA_H
#define DRAG_MEDIA_H

#include "utility/core.h"
#include "editor/core/mouse/DragMouseEvent.h"

class DragMedia : public DragMouseEvent
{
public:

    const sf::Color media_color;
    VideoTime start_time = 0;
    VideoTime length = 0;
    bool valid = false;

    DragMedia(sf::Color color);

    virtual void create_node(GLNode* parent) override;
};

#endif
#ifndef DRAG_MEDIA_H
#define DRAG_MEDIA_H

#include "utility/core.h"
#include "editor/core/DragMouse.h"

class DragMedia : public DragMouse
{
public:

    const sf::Color media_color;
    TimelineUnit start_time = 0;
    TimelineUnit length = 0;
    bool valid = false;

    DragMedia(sf::Color color);

    virtual void create_node(GLNode* parent) override;
};

#endif
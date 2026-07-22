#ifndef DRAG_DIVIDER_H
#define DRAG_DIVIDER_H

#include "editor/core/DragMouse.h"

class DragDivider : public DragMouse
{
    bool vertical;

public:

    DragDivider(sf::Vector2i source_pos, bool vertical);
    bool is_vertical();
};

#endif
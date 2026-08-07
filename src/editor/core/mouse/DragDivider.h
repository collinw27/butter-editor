#ifndef DRAG_DIVIDER_H
#define DRAG_DIVIDER_H

#include "editor/core/mouse/DragMouseEvent.h"

class DragDivider : public DragMouseEvent
{
    bool vertical;

public:

    DragDivider(bool vertical);
    bool is_vertical();
};

#endif
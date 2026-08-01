#ifndef DRAG_DIVIDER_H
#define DRAG_DIVIDER_H

#include "editor/core/DragMouse.h"

class DragDivider : public DragMouse
{
    bool vertical;

public:

    DragDivider(bool vertical);
    bool is_vertical();
};

#endif
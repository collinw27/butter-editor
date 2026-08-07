#ifndef DRAG_SCROLL_H
#define DRAG_SCROLL_H

#include "editor/core/mouse/DragMouseEvent.h"
#include "editor/core/EditorModule.h"

class DragScroll : public DragMouseEvent
{
    float scroll_offset;

public:

    DragScroll(EditorModule* target_module, float scroll_offset);

    float get_scroll_offset();
};

#endif
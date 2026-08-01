#ifndef DRAG_SCROLL_H
#define DRAG_SCROLL_H

#include "editor/core/DragMouse.h"
#include "editor/core/EditorModule.h"

class DragScroll : public DragMouse
{
    float scroll_offset;

public:

    DragScroll(EditorModule* target_module, float scroll_offset);

    float get_scroll_offset();
};

#endif
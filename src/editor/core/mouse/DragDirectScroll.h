#ifndef DRAG_DIRECT_SCROLL_H
#define DRAG_DIRECT_SCROLL_H

#include "editor/core/mouse/DragMouseEvent.h"
#include "editor/core/EditorModule.h"

// This class differs from `DragScroll` since it is used for scroll events
// where the mouse directly drags on the interface (instead of a scroll bar)
// These types of events usually use the middle mouse button

class DragDirectScroll : public DragMouseEvent
{
    // `start_pct` used in some classes to make arithmetic easier

    int start_pos;
    float start_pct;

public:

    DragDirectScroll(EditorModule* target_module, int start_pos, float start_pct);

    int get_start_pos();
    float get_start_pct();
};

#endif
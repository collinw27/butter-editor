#include "editor/core/DragScroll.h"

DragScroll::DragScroll(EditorModule* target_module, float scroll_offset)
{
    this->scroll_offset = scroll_offset;
    target = target_module;
}

float DragScroll::get_scroll_offset()
{
    return scroll_offset;
}
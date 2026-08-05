#include "editor/core/DragDirectScroll.h"

DragDirectScroll::DragDirectScroll(EditorModule* target_module, int start_pos, float start_pct)
{
    this->start_pos = start_pos;
    this->start_pct = start_pct;
    target = target_module;
}

int DragDirectScroll::get_start_pos()
{
    return start_pos;
}

float DragDirectScroll::get_start_pct()
{
    return start_pct;
}
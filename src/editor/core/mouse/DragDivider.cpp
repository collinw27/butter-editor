#include "editor/core/mouse/DragDivider.h"

DragDivider::DragDivider(bool vertical) :
    vertical{vertical}
{}

bool DragDivider::is_vertical()
{
    return vertical;
}
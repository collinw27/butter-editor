#include "editor/core/DragDivider.h"

DragDivider::DragDivider(bool vertical) :
    vertical{vertical}
{}

bool DragDivider::is_vertical()
{
    return vertical;
}
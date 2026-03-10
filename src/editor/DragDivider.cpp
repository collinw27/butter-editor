#include "editor/DragDivider.h"

DragDivider::DragDivider(sf::Vector2i source_pos, bool vertical) :
    DragMouse{source_pos}, vertical{vertical}
{
}

bool DragDivider::is_vertical()
{
    return vertical;
}
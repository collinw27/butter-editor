#include "editor/DragMouse.h"

class DragDivider : public DragMouse
{
    bool vertical;

public:

    DragDivider(sf::Vector2i source_pos, bool vertical);
    bool is_vertical();
};
#include "editor/core/DragMouse.h"

#include "editor/core/EditorModule.h"

sf::Vector2i DragMouse::get_source_pos()
{
    return source_pos;
}

sf::Vector2i DragMouse::get_current_pos()
{
    return current_pos;
}

sf::Vector2i DragMouse::get_total_offset()
{
    return current_pos - source_pos;
}
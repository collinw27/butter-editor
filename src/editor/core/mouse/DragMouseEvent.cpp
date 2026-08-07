#include "editor/core/mouse/DragMouseEvent.h"

#include "editor/core/EditorModule.h"

sf::Vector2i DragMouseEvent::get_source_pos()
{
    return source_pos;
}

sf::Vector2i DragMouseEvent::get_current_pos()
{
    return current_pos;
}

sf::Vector2i DragMouseEvent::get_total_offset()
{
    return current_pos - source_pos;
}


void DragMouseEvent::update_node(sf::Vector2f position)
{
    if (visible_node != nullptr)
        visible_node->set_position(position);
}

void DragMouseEvent::delete_node()
{
    if (visible_node != nullptr)
        delete visible_node;
    visible_node = nullptr;
}
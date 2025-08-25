#include "editor/EditorModule.h"

EditorModule::EditorModule(Editor &editor, sf::IntRect bounds) :
    editor{editor},
    bounds{bounds.position + sf::Vector2i(5, 5), bounds.size - sf::Vector2i(10, 10)}
{
    visible_rect = sf::RectangleShape(sf::Vector2f(this->bounds.size));
    visible_rect.setPosition(sf::Vector2f(this->bounds.position));
    visible_rect.setFillColor(sf::Color::Transparent);
    visible_rect.setOutlineColor(sf::Color(50, 50, 50));
    visible_rect.setOutlineThickness(2);
}

void EditorModule::update()
{
}

void EditorModule::draw(sf::RenderWindow &window)
{
    window.draw(visible_rect);
}

void EditorModule::set_highlight(bool hover, bool drag)
{
    visible_rect.setOutlineColor(hover ? (drag ? C_DRAG : C_HOVER) : C_DEFAULT);
}

void EditorModule::on_mouse_moved(sf::Vector2i position, bool focused)
{
}

void EditorModule::on_mouse_pressed(sf::Vector2i position, bool focused)
{
}

void EditorModule::on_mouse_released(sf::Vector2i position, bool focused, DragMouse *drag_mouse_event)
{
}

sf::IntRect EditorModule::get_rect()
{
    return bounds;
}
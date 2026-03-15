#include "editor/EditorModule.h"

// Bounds are not initialized until `set_bounds()` is called (by Editor)

EditorModule::EditorModule(Editor &editor) :
    editor{editor}
{
    visible_rect = sf::RectangleShape(sf::Vector2f(1, 1));
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

void EditorModule::set_hover_highlight(bool hover)
{
    mouse_hover = hover;
    visible_rect.setOutlineColor(mouse_hover ? C_HOVER : C_DEFAULT);
}

void EditorModule::set_bounds(const sf::IntRect& bounds)
{
    this->bounds = sf::IntRect{bounds.position + sf::Vector2i(5, 5), bounds.size - sf::Vector2i(10, 10)};
    visible_rect.setSize(sf::Vector2f(this->bounds.size));
    visible_rect.setPosition(sf::Vector2f(this->bounds.position));
}

sf::IntRect EditorModule::get_bounds()
{
    return bounds;
}
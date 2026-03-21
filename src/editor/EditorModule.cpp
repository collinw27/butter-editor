#include "editor/EditorModule.h"

#include "editor/Editor.h"

// Bounds are not initialized until `set_bounds()` is called (by Editor)

EditorModule::EditorModule(Editor &editor) :
    editor{editor}
{
    visible_rect = sf::RectangleShape(sf::Vector2f(1, 1));
    visible_rect.setFillColor(sf::Color::Transparent);
    visible_rect.setOutlineColor(Editor::C_HOVER);
    visible_rect.setOutlineThickness(2);
}

void EditorModule::draw(sf::RenderWindow &window)
{
    window.draw(visible_rect);
}

void EditorModule::set_hover_highlight(bool hover)
{
    mouse_hover = hover;
    visible_rect.setOutlineColor(mouse_hover ? Editor::C_HOVER : Editor::C_FG);
}

void EditorModule::set_bounds(const sf::IntRect& new_bounds)
{
    bounds = sf::IntRect{new_bounds.position + sf::Vector2i(5, 5), new_bounds.size - sf::Vector2i(10, 10)};
    visible_rect.setSize(sf::Vector2f(this->bounds.size));
    visible_rect.setPosition(sf::Vector2f(this->bounds.position));
}

void EditorModule::set_ui_scale(float new_scale)
{
    ui_scale = new_scale;
}

sf::IntRect EditorModule::get_bounds()
{
    return bounds;
}
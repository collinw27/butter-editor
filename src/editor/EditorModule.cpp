#include "editor/EditorModule.h"

#include "editor/Editor.h"

// Bounds are not initialized until `set_bounds()` is called (by Editor)

EditorModule::EditorModule(Editor &editor) :
    editor{editor}
{
    visible_rect = GLOutlinedRectangle::create(nullptr);
    visible_rect->set_fill_color(sf::Color::Transparent);
    visible_rect->set_outline_color(Editor::C_HOVER);
    visible_rect->set_outline_thickness(2);
}

GLNode* EditorModule::get_node()
{
    return visible_rect;
}

void EditorModule::set_bounds(const sf::IntRect& new_bounds)
{
    bounds = sf::IntRect{new_bounds.position + sf::Vector2i(5, 5), new_bounds.size - sf::Vector2i(10, 10)};
    visible_rect->set_size(sf::Vector2f(this->bounds.size));
    visible_rect->set_position(sf::Vector2f(this->bounds.position));
}

void EditorModule::set_ui_scale(float new_scale)
{
    ui_scale = new_scale;
}

void EditorModule::set_visible(bool visible)
{
    visible_rect->set_visible(visible);
}

void EditorModule::set_hover_highlight(bool hover)
{
    mouse_hover = hover;
    visible_rect->set_outline_color(mouse_hover ? Editor::C_HOVER : Editor::C_FG);
}

sf::IntRect EditorModule::get_bounds()
{
    return bounds;
}
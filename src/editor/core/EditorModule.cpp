#include "editor/core/EditorModule.h"

#include "editor/Editor.h"

// Bounds are not initialized until `set_bounds()` is called (by Editor)

EditorModule::EditorModule(Editor &editor) :
    editor{editor}
{
    visible_rect.reset(GLOutlinedRectangle::create(nullptr));
    visible_rect->set_fill_color(Editor::C_BG);
    visible_rect->set_outline_color(Editor::C_BORDER);
    visible_rect->set_outline_thickness(2);
    container.reset(GLContainer::create(visible_rect.get(), sf::Vector2f(), sf::Vector2f(bounds.size)));
    notif_classes.clear();
}

GLNode* EditorModule::get_node()
{
    return visible_rect.get();
}

sf::IntRect EditorModule::get_bounds()
{
    return bounds;
}

void EditorModule::set_bounds(sf::IntRect new_bounds)
{
    bounds = sf::IntRect{new_bounds.position + sf::Vector2i(5, 5), new_bounds.size - sf::Vector2i(10, 10)};
    visible_rect->set_size(sf::Vector2f(this->bounds.size));
    visible_rect->set_position(sf::Vector2f(this->bounds.position));
    container->set_size(sf::Vector2f(bounds.size));
    apply_bounds();
}

void EditorModule::set_ui_scale(float new_scale)
{
    ui_scale = new_scale;
    apply_ui_scale();
}

sf::Vector2i EditorModule::to_local_pos(sf::Vector2i position)
{
    return position - bounds.position;
}

void EditorModule::set_visible(bool visible)
{
    visible_rect->set_visible(visible);
}

void EditorModule::set_focused(bool focused)
{
    has_focus = focused;
    visible_rect->set_outline_color(has_focus ?
        (mouse_hover ? Editor::C_FOCUSED_HOVER : Editor::C_FOCUSED) :
        (mouse_hover ? Editor::C_BORDER_HOVER : Editor::C_BORDER)
    );
}

void EditorModule::set_hover_highlight(bool hover)
{
    mouse_hover = hover;
    visible_rect->set_outline_color(has_focus ?
        (mouse_hover ? Editor::C_FOCUSED_HOVER : Editor::C_FOCUSED) :
        (mouse_hover ? Editor::C_BORDER_HOVER : Editor::C_BORDER)
    );
}

bool EditorModule::receives_notifs(int notif_class)
{
    return (std::find(notif_classes.begin(), notif_classes.end(), notif_class) != notif_classes.end());
}

void EditorModule::opt_into_notifs(int notif_class)
{
    if (std::find(notif_classes.begin(), notif_classes.end(), notif_class) == notif_classes.end())
        notif_classes.push_back(notif_class);
}
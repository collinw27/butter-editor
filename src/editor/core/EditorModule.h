#ifndef EDITOR_MODULE_H
#define EDITOR_MODULE_H

#include <memory>
#include "utility/Input.h"
#include "graphics/nodes.h"

class Editor;
class DragMouse;

class EditorModule
{
protected:

    Editor& editor;
    sf::IntRect bounds;
    float ui_scale = 1.f;
    std::unique_ptr<GLOutlinedRectangle> visible_rect;
    std::unique_ptr<GLContainer> container;
    bool mouse_hover;

public:

    EditorModule(Editor& editor);
    virtual void update() {}
    GLNode* get_node();

    sf::IntRect get_bounds();
    void set_bounds(sf::IntRect new_bounds);
    virtual void apply_bounds() {}
    void set_ui_scale(float new_scale);
    virtual void apply_ui_scale() {}

    sf::Vector2i to_local_pos(sf::Vector2i position);
    
    void set_visible(bool visible);
    void set_hover_highlight(bool hover);

    // `update()` is called every frame
    // When possible, behavior should instead be delegated to mouse functions

    virtual void on_update() {}

    // Allow custom behavior on mouse inputs
    // These functions will be triggered even when the mouse is outside the module bounds
    // Because of this, `focused` should be checked in cases where an event should only
    // trigger when the cursor is in this module
    // `position` is given relative to the top-left of the module
    // `event` is only passed if the current mouse event is targeted toward this module
    
    virtual void on_mouse_press(sf::Vector2i position, bool focused, InputButton button) {}
    virtual void on_mouse_move(sf::Vector2i position, bool focused, DragMouse* drag_event) {}
    virtual void on_mouse_release(sf::Vector2i position, bool focused, InputButton button, DragMouse* drag_event) {}

    // This event only triggers when a mouse event is released over this module

    virtual void on_mouse_drop(sf::Vector2i position, DragMouse* drag_event) {}
};

#endif
#ifndef EDITOR_MODULE_H
#define EDITOR_MODULE_H

#include "graphics/nodes.h"

class Editor;
class DragMouse;

class EditorModule
{
protected:

    Editor& editor;
    sf::IntRect bounds;
    float ui_scale = 1.f;
    GLOutlinedRectangle* visible_rect;
    GLContainer* container;
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
    
    void set_visible(bool visible);
    void set_hover_highlight(bool hover);

    // Allow custom behavior on mouse inputs
    // These functions will be triggered even when the mouse is outside the module bounds
    // Because of this, `focused` should be checked in cases where an event should only
    // trigger when the cursor is in this module
    // `position` is given relative to the top-left of the module
    // `event` is only passed if the current mouse event is targeted toward this module
    
    virtual void on_mouse_press(sf::Vector2i position, bool focused) {}
    virtual void on_mouse_move(sf::Vector2i position, bool focused, DragMouse* drag_event) {}
    virtual void on_mouse_release(sf::Vector2i position, bool focused, DragMouse* drag_event) {}
};

#endif
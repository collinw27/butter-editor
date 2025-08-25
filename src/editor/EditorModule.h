#ifndef EDITOR_MODULE_H
#define EDITOR_MODULE_H

#include <SFML/Graphics.hpp>

#include "editor/DragMouse.h"

class Editor;

class EditorModule
{
protected:

    const sf::Color C_DEFAULT{50, 50, 50};
    const sf::Color C_HOVER{80, 80, 80};
    const sf::Color C_DRAG{84, 113, 255};

    Editor &editor;
    sf::IntRect bounds;
    sf::RectangleShape visible_rect;

public:

    EditorModule(Editor &editor, sf::IntRect bounds);
    virtual void update();
    virtual void draw(sf::RenderWindow &window);

    void set_highlight(bool hover, bool drag);

    // Allow custom behavior on mouse inputs
    // These functions will be triggered even when the mouse is outside the module bounds
    // Because of this, `focused` should be checked in cases where an event should only
    // trigger when the cursor is in this module
    
    virtual void on_mouse_moved(sf::Vector2i position, bool focused);
    virtual void on_mouse_pressed(sf::Vector2i position, bool focused);
    virtual void on_mouse_released(sf::Vector2i position, bool focused, DragMouse *drag_mouse_event);

    sf::IntRect get_rect();
};

#endif
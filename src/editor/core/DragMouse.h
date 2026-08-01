#ifndef MOUSE_DRAG_H
#define MOUSE_DRAG_H

#include <SFML/Graphics.hpp>

class Editor;
class EditorModule;

class DragMouse
{
    sf::Vector2i source_pos;
    sf::Vector2i current_pos;

    // A target can be used to send updates to a module
    // This can also be nullptr to not send any updates
    // Setting a target is only recommended for actions that stay within
    // one module. For example:
    // Dragging a scroll bar: Stays within target module
    // Dragging clip from media module: Doesn't stay within module
    // Dragging a divider between modules: Doesn't belong to any module

protected:

    EditorModule* target = nullptr;

public:

    DragMouse() {}
    virtual ~DragMouse() {}

    sf::Vector2i get_source_pos();
    sf::Vector2i get_current_pos();
    sf::Vector2i get_total_offset();

    friend class Editor;
    
private:

    // These functions should be called by the Editor
    
    virtual void on_move() {}
    virtual void on_release() {}
    virtual void draw(sf::RenderWindow& window) {}
};

#endif
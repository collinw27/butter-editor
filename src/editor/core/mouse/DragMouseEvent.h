#ifndef MOUSE_DRAG_H
#define MOUSE_DRAG_H

#include <SFML/Graphics.hpp>
#include "graphics/GLNode.h"

class Editor;
class EditorModule;

class DragMouseEvent
{
    sf::Vector2i source_pos;
    sf::Vector2i current_pos;

protected:

    // A target can be used to send updates to a module
    // This can also be nullptr to not send any updates
    // Setting a target is only recommended for actions that stay within
    // one module. For example:
    // Dragging a scroll bar: Stays within target module
    // Dragging clip from media module: Doesn't stay within module
    // Dragging a module divider: Doesn't belong to any module

    EditorModule* target = nullptr;
    GLNode* visible_node = nullptr;

public:

    DragMouseEvent() {}
    virtual ~DragMouseEvent() {}

    sf::Vector2i get_source_pos();
    sf::Vector2i get_current_pos();
    sf::Vector2i get_total_offset();

    friend class Editor;
    
private:

    // These functions should be called by the Editor
    
    virtual void on_move() {}
    virtual void on_release() {}

    // This class has the option to create a node that will follow the cursor
    // `create_node()` should create the node and store it in `visible_node`
    // The node will be automatically freed by the Editor when 

    virtual void create_node(GLNode* parent) {}
    void update_node(sf::Vector2f position);
    void delete_node();
};

#endif
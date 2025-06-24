#ifndef TIMELINE_MODULE_H
#define TIMELINE_MODULE_H

#include <string>
#include <vector>

#include "editor/EditorModule.h"

class TimelineModule : public EditorModule
{
    sf::RectangleShape timelineRect;

public:

    TimelineModule(Editor &editor);
    virtual void draw(sf::RenderWindow &window) override;
    
    virtual void on_mouse_released(sf::Vector2i position, bool focused, DragMouse *drag_mouse_event) override;
};

#endif
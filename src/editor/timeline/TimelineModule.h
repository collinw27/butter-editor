#ifndef TIMELINE_MODULE_H
#define TIMELINE_MODULE_H

#include <string>
#include <vector>

#include "editor/EditorModule.h"
#include "editor/timeline/TimelineClip.h"

// For the time being, the timeline will hold clips
// of length 0.1 ranging from a range of 0-1 on the timeline
// This will give a way of testing the UI implementation
// and make implementing images/videos easier later on

class TimelineModule : public EditorModule
{

    sf::RectangleShape timeline_rect;
    std::vector<TimelineClip*> media_clips;
    TimelineClip *selected_clip = nullptr;
    std::vector<TimelineClip*> deleted_clips;

public:

    TimelineModule(Editor &editor);
    virtual void update() override;
    virtual void draw(sf::RenderWindow &window) override;
    
    virtual void on_mouse_pressed(sf::Vector2i position, bool focused) override;
    virtual void on_mouse_released(sf::Vector2i position, bool focused, DragMouse *drag_mouse_event) override;

    sf::FloatRect get_clip_bounds(float start_time, float length);
    void remove_clip(TimelineClip *clip);
};

#endif
#ifndef TIMELINE_MODULE_H
#define TIMELINE_MODULE_H

#include <string>
#include <vector>

#include "graphics/Node.h"
#include "graphics/RectNode.h"
#include "editor/EditorModule.h"
#include "editor/timeline/TimelineClip.h"

class TimelineModule : public EditorModule
{
    sf::RectangleShape timeline_rect;
    std::vector<TimelineClip*> media_clips;
    std::vector<TimelineClip*> deleted_clips;
    TimelineClip *selected_clip = nullptr;
    Node *clip_root;

    // Lengths and times in the editor are measured by frame number
    // For now, the project will assume a framerate of 60 FPS
    // At the maximum zoom (10), 1 frame = 100 pixels wide
    // `h_zoom` is frame_width/10
    // `h_scroll` is the frame # on the very left

    int total_length = 0;
    float h_zoom = 1.f;
    float h_scroll = 0.f;

public:

    TimelineModule(Editor &editor);
    virtual void update() override;
    virtual void draw(sf::RenderWindow &window) override;
    
    virtual void on_mouse_pressed(sf::Vector2i position, bool focused) override;
    virtual void on_mouse_released(sf::Vector2i position, bool focused, DragMouse *drag_mouse_event) override;

    void remove_clip(TimelineClip *clip);
};

#endif
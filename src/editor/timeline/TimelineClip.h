#ifndef TIMELINE_CLIP_H
#define TIMELINE_CLIP_H

#include <memory>
#include "utility/core.h"
#include "project/clip/Clip.h"
#include "project/Project.h"
#include "graphics/GLRectangle.h"

// The state of each instance should reflect the state of the clip
// within the Project instance
// Because of this, actions within the timeline shouldn't affect this
// class directly, but instead should modify the Project
// Then, changes from the project propgate to this class using the
// TimelineModule's notification system

class TimelineClip
{
public:

    const id_s clip_id;

private:

    std::unique_ptr<GLRectangle> rect;
    std::unique_ptr<GLRectangle> border;
    sf::Color thumbnail_color;
    bool is_selected = false;
    VideoTime clip_start = 0;
    VideoTime clip_length = 0;

public:

    TimelineClip(id_s clip_id, GLNode* container);
    virtual ~TimelineClip() {}

    GLRectangle* get_rect();
    GLRectangle* get_border();

    bool selected();
    void render_selected(GLNode* container, float t_scale);
    void deselect();
    void set_hovering(bool hovering);

    bool is_time_within(int time);
    bool is_start_within(float left, float right);
    bool is_end_within(float left, float right);

    void set_clip_start(VideoTime start);
    void set_clip_length(VideoTime length);
    void set_clip_end(VideoTime end);
    void set_thumbnail_color(sf::Color color);
};

#endif
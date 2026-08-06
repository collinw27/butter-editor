#ifndef CLIP_H
#define CLIP_H

#include <memory>
#include "project/clip/ClipData.h"
#include "project/Project.h"
#include "graphics/GLRectangle.h"

// This class has two main responsibilities:
// 1) Displaying the timeline clip using GLNodes
// 2) Propogating any change to this clip to the project interface

class Clip
{
    sf::Color color;
    std::unique_ptr<GLRectangle> rect;
    std::unique_ptr<GLRectangle> border;
    ClipData* clip_data;
    bool is_selected = false;

public:

    Clip(ClipData* clip_data, sf::Color color, GLNode* container);
    virtual ~Clip() {}

    GLRectangle* get_rect();
    GLRectangle* get_border();
    const ClipData* get_clip_data();

    bool selected();
    void render_selected(GLNode* container, float t_scale);
    void deselect();
    void set_hovering(bool hovering);

    bool is_time_within(int time);
    bool is_start_within(float left, float right);
    bool is_end_within(float left, float right);

    void set_clip_start(Project* project, TimelineUnit start);
    void set_clip_end(Project* project, TimelineUnit end);
    void delete_clip(Project* project);
};

#endif
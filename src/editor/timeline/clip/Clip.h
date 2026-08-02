#ifndef CLIP_H
#define CLIP_H

#include <memory>
#include "project/clip/ClipData.h"
#include "project/Project.h"
#include "graphics/glRectangle.h"

// This class has two main responsibilities:
// 1) Displaying the timeline clip using GLNodes
// 2) Propogating any change to this clip to the project interface
// This class should be extended for all subclasses of TimelineClip

class Clip
{
    std::unique_ptr<GLRectangle> rect;
    std::unique_ptr<GLRectangle> border;
    ClipData* clip_data;
    bool is_selected = false;

public:

    Clip(ClipData* clip_data, sf::Color color, GLNode* container);
    virtual ~Clip() {}

    bool selected();
    void select(GLNode* container);
    void deselect();

    void delete_clip(Project* project);
};

#endif
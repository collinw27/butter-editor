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
    ClipData* clip_data;

public:

    Clip(ClipData* clip_data, sf::Color color, GLNode* container);
    virtual ~Clip() {}

    void delete_clip(Project* project);
};

#endif
#ifndef EMPTY_CLIP_H
#define EMPTY_CLIP_H

#include "project/timeline/TimelineClip.h"
#include "utility/core.h"

// This clip isn't actually visible/modifiable on the timeline
// Instead, because of the way clips are stored in the project,
// the beginning of this clip is simply used to denote where
// the previous clip ends

class EmptyClip : public TimelineClip
{
public:

    EmptyClip(TimelinePos start_time);

    virtual int get_clip_type() override;
};

#endif
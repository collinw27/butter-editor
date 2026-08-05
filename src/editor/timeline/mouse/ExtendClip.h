#ifndef EXTEND_CLIP_H
#define EXTEND_CLIP_H

#include "utility/core.h"
#include "editor/core/DragMouse.h"

class Clip;

class ExtendClip : public DragMouse
{
public:

    const bool forward;
    const TimelineUnit start_time;
    const TimelineUnit max_extend;
    const TimelineUnit max_trim;

    // The Clip* is not stored here to avoid the potential of a dangling pointer
    
    ExtendClip(EditorModule* target_module, bool forward, TimelineUnit start_time, TimelineUnit max_extend, TimelineUnit max_trim);
};

#endif
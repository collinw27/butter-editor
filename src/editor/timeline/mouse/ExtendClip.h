#ifndef EXTEND_CLIP_H
#define EXTEND_CLIP_H

#include "utility/core.h"
#include "editor/core/mouse/DragMouseEvent.h"

class Clip;

class ExtendClip : public DragMouseEvent
{
public:

    const bool forward;
    const VideoTime start_time;
    const VideoTime max_extend;
    const VideoTime max_trim;

    // The Clip* is not stored here to avoid the potential of a dangling pointer
    
    ExtendClip(EditorModule* target_module, bool forward, VideoTime start_time, VideoTime max_extend, VideoTime max_trim);
};

#endif
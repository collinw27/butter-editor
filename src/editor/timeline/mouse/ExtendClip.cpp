#include "editor/timeline/mouse/ExtendClip.h"

ExtendClip::ExtendClip(EditorModule* target_module, bool forward, TimelineUnit start_time, TimelineUnit max_extend, TimelineUnit max_trim) :
    forward{forward},
    start_time{start_time},
    max_extend{max_extend},
    max_trim{max_trim}
{
    target = target_module;
}
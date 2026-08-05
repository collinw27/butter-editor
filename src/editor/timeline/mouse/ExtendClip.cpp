#include "editor/timeline/mouse/ExtendClip.h"

ExtendClip::ExtendClip(EditorModule* target_module, bool forward, TimelineUnit start_time, TimelineUnit max_extend, int start_pos) :
    forward{forward},
    start_time{start_time},
    max_extend{max_extend},
    start_pos{start_pos}
{
    target = target_module;
}
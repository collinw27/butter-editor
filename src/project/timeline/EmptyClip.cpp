#include "project/timeline/EmptyClip.h"

EmptyClip::EmptyClip(TimelinePos start_time)
    : TimelineClip(start_time) {}

int EmptyClip::get_clip_type()
{
    return (int) TimelineClipType::EMPTY;
}
#ifndef TIMELINE_OBJECT_H
#define TIMELINE_OBJECT_H

#include <fstream>
#include "utility/core.h"

// Enum for serializing clips
// Having to store all subclasses in this file feels weird, but
// an enum is the easiest way to prevent ID collisions

enum class TimelineClipType
{
    EMPTY,
    COLOR
};

class TimelineClip
{
    TimelinePos start_time;

public:

    TimelineClip(TimelinePos start_time);
    virtual ~TimelineClip();

    virtual int get_clip_type() = 0;
    TimelinePos get_start_time();

    virtual void save(std::ofstream& file);
};

#endif
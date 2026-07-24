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
    TimelineUnit start_time;
    TimelineUnit length;

public:

    TimelineClip(TimelineUnit start_time, TimelineUnit length);
    virtual ~TimelineClip();

    virtual int get_clip_type() = 0;
    TimelineUnit get_start_time();
    TimelineUnit get_end_time();
    TimelineUnit get_length();

    virtual void save(std::ofstream& file);
};

#endif
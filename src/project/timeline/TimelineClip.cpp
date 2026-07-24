#include "project/timeline/TimelineClip.h"

TimelineClip::TimelineClip(TimelineUnit start_time, TimelineUnit length)
{
    this->start_time = start_time;
    this->length = length;
}

TimelineClip::~TimelineClip() {}

TimelineUnit TimelineClip::get_start_time()
{
    return start_time;
}

TimelineUnit TimelineClip::get_end_time()
{
    return start_time + length;
}

TimelineUnit TimelineClip::get_length()
{
    return length;    
}

void TimelineClip::save(std::ofstream& file) {}
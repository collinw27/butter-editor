#include "project/timeline/TimelineClip.h"

#include "utility/core.h"

TimelineClip::TimelineClip(TimelineUnit start_time, TimelineUnit length)
{
    this->start_time = start_time;
    this->length = length;
}

TimelineClip::~TimelineClip() {}

void TimelineClip::set_start_time(TimelineUnit start_time)
{
    this->start_time = start_time;
}

TimelineUnit TimelineClip::get_start_time()
{
    return start_time;
}

void TimelineClip::set_end_time(TimelineUnit end_time)
{
    if (end_time <= start_time)
        throw ButterException("Clip length must be positive");
    length = end_time - start_time;
}

TimelineUnit TimelineClip::get_end_time()
{
    return start_time + length;
}

void TimelineClip::set_length(TimelineUnit length)
{
    if (length <= 0)
        throw ButterException("Clip length must be positive");
    this->length = length;
}

TimelineUnit TimelineClip::get_length()
{
    return length;    
}

void TimelineClip::save(std::ofstream& file) {}
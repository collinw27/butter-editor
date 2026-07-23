#include "project/timeline/TimelineClip.h"

TimelineClip::TimelineClip(TimelinePos start_time)
{
    this->start_time = start_time;
}

TimelineClip::~TimelineClip() {}

TimelinePos TimelineClip::get_start_time()
{
    return start_time;
}

void TimelineClip::save(std::ofstream& file) {}
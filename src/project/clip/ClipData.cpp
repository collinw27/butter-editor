#include "project/clip/ClipData.h"

#include "utility/core.h"

ClipData::ClipData(TimelineUnit start_time, TimelineUnit length)
{
    this->start_time = start_time;
    this->length = length;
}

ClipData::~ClipData() {}

void ClipData::set_start_time(TimelineUnit start_time)
{
    this->start_time = start_time;
}

TimelineUnit ClipData::get_start_time()
{
    return start_time;
}

void ClipData::set_end_time(TimelineUnit end_time)
{
    if (end_time <= start_time)
        throw ButterException("Clip length must be positive");
    length = end_time - start_time;
}

TimelineUnit ClipData::get_end_time()
{
    return start_time + length;
}

void ClipData::set_length(TimelineUnit length)
{
    if (length <= 0)
        throw ButterException("Clip length must be positive");
    this->length = length;
}

TimelineUnit ClipData::get_length()
{
    return length;    
}

void ClipData::save(std::ofstream& file) {}
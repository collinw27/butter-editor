#include "project/clip/ClipData.h"

#include "utility/core.h"

ClipData::ClipData(VideoTime start_time, VideoTime length)
{
    this->start_time = start_time;
    this->length = length;
}

ClipData::~ClipData() {}

void ClipData::set_start_time(VideoTime start_time)
{
    this->start_time = start_time;
}

VideoTime ClipData::get_start_time() const
{
    return start_time;
}

void ClipData::set_end_time(VideoTime end_time)
{
    if (end_time <= start_time)
        throw ButterException("Clip length must be positive");
    length = end_time - start_time;
}

VideoTime ClipData::get_end_time() const
{
    return start_time + length;
}

void ClipData::set_length(VideoTime length)
{
    if (length <= 0)
        throw ButterException("Clip length must be positive");
    this->length = length;
}

VideoTime ClipData::get_length() const
{
    return length;    
}

void ClipData::save(std::ofstream& file) {}
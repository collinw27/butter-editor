#include "project/clip/Clip.h"

#include "utility/core.h"

Clip::Clip(VideoTime start_time, VideoTime length, id_s id)
    : id{id}
{
    this->start_time = start_time;
    this->length = length;
}

Clip::~Clip() {}

void Clip::set_start_time(VideoTime start_time)
{
    this->start_time = start_time;
}

VideoTime Clip::get_start_time() const
{
    return start_time;
}

void Clip::set_end_time(VideoTime end_time)
{
    if (end_time <= start_time)
        throw ButterException("Clip length must be positive");
    length = end_time - start_time;
}

VideoTime Clip::get_end_time() const
{
    return start_time + length;
}

void Clip::set_length(VideoTime length)
{
    if (length <= 0)
        throw ButterException("Clip length must be positive");
    this->length = length;
}

VideoTime Clip::get_length() const
{
    return length;    
}

void Clip::save(std::ofstream& file) {}
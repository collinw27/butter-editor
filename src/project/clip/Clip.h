#ifndef CLIP_H
#define CLIP_H

#include <fstream>
#include "utility/core.h"

// Enum for serializing clips
// Having to store all subclasses in this file feels weird, but
// an enum is the easiest way to prevent ID collisions

enum class ClipType
{
    EMPTY,
    COLOR
};

class Clip
{
public:

    const id_s id;

private:

    VideoTime start_time;
    VideoTime length;

public:

    Clip(VideoTime start_time, VideoTime length, id_s id);
    virtual ~Clip();

    virtual int get_clip_type() = 0;
    void set_start_time(VideoTime start_time);
    VideoTime get_start_time() const;
    void set_end_time(VideoTime end_time);
    VideoTime get_end_time() const;
    void set_length(VideoTime length);
    VideoTime get_length() const;

    virtual void save(std::ofstream& file);
};

#endif
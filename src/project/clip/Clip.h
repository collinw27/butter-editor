#ifndef CLIP_H
#define CLIP_H

#include <fstream>
#include "utility/core.h"
#include "project/types.h"
#include "graphics/GLTexture.h"

// Quick note on conventions:
// Often times, a clip will need to interface with a media object
// in order to render itself or retrieve its thumbnail (ex. ImageClip)
// In this case, a reference is stored to the ImageMedia directly instead
// of storing its ID, since retrieving data from an ID requires using the
// Project class and therefore coupling it with the ImageClip class

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

    void set_start_time(VideoTime start_time);
    VideoTime get_start_time() const;
    void set_end_time(VideoTime end_time);
    VideoTime get_end_time() const;
    void set_length(VideoTime length);
    VideoTime get_length() const;

    virtual ClipType get_clip_type() = 0;
    virtual const GLTexture* get_thumbnail() = 0;
    virtual void save(std::ofstream& file);
};

#endif
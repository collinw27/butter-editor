#ifndef CLIP_H
#define CLIP_H

#include <fstream>
#include "utility/core.h"
#include "project/types.h"
#include "graphics/texture/GLTexture.h"
#include "graphics/GLFrameBuffer.h"

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
    
    // All frame function assume the buffer's OpenGL context has already been activated
    // To avoid re-creating objects that persist between changed frames, frame rendering
    // logic is broken up into 3 functions:
    // `enter_frame()`: Creates the nodes that are needed for the draw function
    // `exit_frame()`: Deletes any existing nodes when this clip is no longer visible
    // `update_frame()`: Configures the nodes to the correct display (also called after `enter_frame()`)
    // It's possible that clip types will be created in the future that more complex logic,
    // but these can be addressed with a `change_frame(prev_time, time)` function
    
    virtual void enter_frame(GLFrameBuffer* buffer) {}
    virtual void exit_frame(GLFrameBuffer* buffer) {}
    virtual void update_frame(GLFrameBuffer* buffer, VideoTime time) {}
};

#endif
#ifndef EXPORT_TASK_H
#define EXPORT_TASK_H

#include <thread>
#include <mutex>
#include <subprocess.hpp>
#include "utility/core.h"

struct ExportTask
{
    subprocess::Popen ffmpeg_pipe;
    std::thread thread;
    std::size_t buffer_size;
    std::uint8_t* buffer;
    TimelineUnit final_frame;
};

#endif
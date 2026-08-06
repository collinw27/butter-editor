#include "project/Project.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>

#include "utility/core.h"
#include "utility/FileManager.h"
#include "utility/Logger.h"
#include "project/exceptions.h"

#include "project/clip/ColorClipData.h"

void export_async();

LockedProject::LockedProject() : basic_mutex{} {}

LockedProject::~LockedProject() {}

bool LockedProject::is_exporting()
{
    std::lock_guard<std::mutex> lock {basic_mutex};
    return !export_finished;
}

int LockedProject::get_export_percentage()
{
    std::lock_guard<std::mutex> lock {basic_mutex};
    return export_finished ? 100 : export_progress;
}

Project::Project() : LockedProject{}
{
    name = std::nullopt;

    // Use default project parameters

    framerate = 30;
    resolution = sf::Vector2u(400, 300);
}

Project::Project(std::string name) : LockedProject{}
{
    this->name = name;
    
    std::ifstream file {FileManager().get_data_path("projects/" + this->name.value() + ".proj")};
    proj_assert(file.is_open(), "Could not find project");

    // Load project parameters

    int int_buffer;
    file >> int_buffer;
    resolution.x = int_buffer;
    proj_assert(resolution.x > 0, "Invalid resolution");
    file >> int_buffer;
    resolution.y = int_buffer;
    proj_assert(resolution.y > 0, "Invalid resolution");
    file >> int_buffer;
    framerate = int_buffer;
    proj_assert(framerate > 0, "Invalid framerate");

    // Individually load each clip

    int clip_total;
    file >> clip_total;

    int clip_type;
    ClipData* new_clip = nullptr;
    int clip_start;
    int clip_length;

    for (int i = 0; i < clip_total; ++i)
    {
        // Switch statement here is a little ugly
        // std::function can be used to make this more compact but less optimized,
        // so in this case I'll opt to use boilerplate for more efficiency

        file >> clip_type;
        file >> clip_start;
        file >> clip_length;
        switch (static_cast<ClipType>(clip_type))
        {
        case ClipType::COLOR:
            new_clip = new ColorClipData(clip_start, clip_length, file);
        break;
        default:
            throw ButterException("Invalid clip type: " + std::to_string(clip_type));
        }
        clip_vec.push_back(std::unique_ptr<ClipData>(new_clip));
    }

    file.close();
}

Project::~Project() {}

bool Project::named()
{
    return name.has_value();
}

std::string Project::get_name()
{
    return name.has_value() ? name.value() : "Untitled project";
}

void Project::set_name(std::string name)
{
    this->name = name;
}

int Project::get_framerate()
{
    return framerate;
}

sf::Vector2u Project::get_resolution()
{
    return resolution;
}

// Returns whether the operation was successful
// It can fail if there's no space to insert the clip

bool Project::add_color_clip(TimelineUnit start_time, TimelineUnit length, sf::Color color)
{
    // Add the color clip to the timeline
    // Find the first position it can slot in before something
    // If this doesn't exist, insert it at the end

    ColorClipData* new_clip = new ColorClipData(start_time, length, color);
    auto next_clip = clip_vec.begin();
    while (next_clip < clip_vec.end())
    {
        if (start_time < (*next_clip)->get_start_time())
        {
            // The clip afterward can trim the length of the clip
            // (0 length = no clip inserted)

            TimelineUnit new_length = std::min(length, (*next_clip)->get_start_time() - start_time);
            if (new_length <= 0)
                return false;
            new_clip->set_length(new_length);

            // If this passed, proceed to clip inserting logic
            // (Takes place after loop to also account for case where the clip
            // is inserted at the end of the vector)

            break;
        }
        ++next_clip;
    }

    // If the start is placed inside another clip, no clip is created

    if (next_clip != clip_vec.begin())
    {
        auto prev_clip = next_clip - 1;
        if ((*prev_clip)->get_end_time() > start_time)
            return false;
    }

    // If all checks passed, insert clip
    
    clip_vec.insert(next_clip, std::unique_ptr<ClipData>(new_clip));
    return true;
}

void Project::set_clip_start(ClipData* clip, TimelineUnit start)
{
    // This method exits prematurely if this clip will cut into its neighbor
    // or if it will result in a clip with length 0

    auto it = std::find_if(clip_vec.begin(), clip_vec.end(), [clip] (std::unique_ptr<ClipData>& smart_ptr) { return smart_ptr.get() == clip; } );
    if (it == clip_vec.end())
        throw ButterException("Cannot extend clip");
    if (start >= clip->get_end_time())
        return;
    if (it != clip_vec.begin())
    {
        ClipData* clip_before = (it - 1)->get();
        if (start < clip_before->get_end_time())
            return;
    }
    TimelineUnit old_end = clip->get_end_time();
    clip->set_start_time(start);
    clip->set_end_time(old_end);
}

void Project::set_clip_end(ClipData* clip, TimelineUnit end)
{
    // This method exits prematurely if this clip will cut into its neighbor,
    // or if it will result in a clip with length 0

    auto it = std::find_if(clip_vec.begin(), clip_vec.end(), [clip] (std::unique_ptr<ClipData>& smart_ptr) { return smart_ptr.get() == clip; } );
    if (it == clip_vec.end())
        throw ButterException("Cannot extend clip");
    if (end <= clip->get_start_time())
        return;
    if (it + 1 != clip_vec.end())
    {
        ClipData* clip_after = (it + 1)->get();
        if (end > clip_after->get_start_time())
            return;
    }
    clip->set_end_time(end);
}

void Project::delete_clip(ClipData* clip)
{
    auto it = std::find_if(clip_vec.begin(), clip_vec.end(), [clip] (std::unique_ptr<ClipData>& smart_ptr) { return smart_ptr.get() == clip; } );
    if (it == clip_vec.end())
        throw ButterException("Cannot delete clip");
    clip_vec.erase(it);
}

unsigned int Project::get_clip_total()
{
    return clip_vec.size();
}

ClipData* Project::get_clip_at_index(unsigned int index)
{
    try
    {
        return clip_vec.at(index).get();
    }
    catch (std::out_of_range e)
    {
        return nullptr;
    }
}

ClipData* Project::get_clip_at_time(TimelineUnit time)
{
    auto it = get_iter_at_time(time);
    return (it == clip_vec.end()) ? nullptr : (*it).get();
}

TimelineUnit Project::get_project_length()
{
    // Return the end time of the final clip
    // If the timeline is empty, return length of 1

    return (clip_vec.empty() ? 0 : clip_vec.back()->get_end_time());
}

std::string Project::get_project_length_approx()
{
    TimelineUnit time = get_project_length();
    time = (TimelineUnit) std::ceil(time / ((double) framerate));
    TimelineUnit sec = time % 60;
    time = (time - sec) / 60;
    TimelineUnit min = time % 60;
    time = (time - min) / 60;
    TimelineUnit hr = time;
    return (std::stringstream{} << std::setfill('0')
        << std::setw(2) << hr << "h "
        << std::setw(2) << min << "m "
        << std::setw(2) << sec << "s"
    ).str();
}

std::string Project::to_string(TimelineUnit timeline_time)
{
    int time = (int) timeline_time;
    int frame = time % framerate;
    time = (time - frame) / framerate;
    int sec = time % 60;
    time = (time - sec) / 60;
    int min = time % 60;
    time = (time - min) / 60;
    int hr = time;
    return (std::stringstream{} << std::setfill('0')
        << std::setw(2) << hr << ":"
        << std::setw(2) << min << ":"
        << std::setw(2) << sec << "+"
        << std::setw(2) << frame
    ).str();
}

// These functions check how far ahead/behind of a time
// you can continue without running into a clip
// 0 if inside of clip

TimelineUnit Project::get_gap_ahead(TimelineUnit time)
{
    // Find the clip directly after the time (if applicable)
    // This clip is the only thing that can terminate the gap
    // A clip that ends exactly on this frame will not be considered,
    // since a valid gap will still exist after the clip

    int index = -1;
    for (int i = clip_vec.size() - 1; i >= 0; --i)
    {
        if (clip_vec.at(i)->get_end_time() > time)
            index = i;
        else
            break;
    }
    ClipData* after = (index != -1) ? clip_vec.at(index).get() : nullptr;

    // Return the distance to the start of the proceeding clip
    // If the start is before the given time (i.e. the time is inside the clip),
    // we should return 0 (hence the max function; be careful with unsigned integers)
    // If there is no clip after, the gap extends to the end of time timeline

    if (after != nullptr)
        return std::max((int) after->get_start_time() - (int) time, 0);
    else
        return (TIMELINE_MAX - time);
}

TimelineUnit Project::get_gap_behind(TimelineUnit time)
{
    // The logic here is pretty similar to the function above
    // Instead, find the clip immediately before

    int index = -1;
    for (int i = 0; i < clip_vec.size(); ++i)
    {
        if (clip_vec.at(i)->get_start_time() < time)
            index = i;
        else
            break;
    }
    ClipData* before = (index != -1) ? clip_vec.at(index).get() : nullptr;

    // The only difference here is the clip does not extend infinitely;
    // it gets cut off at time 0 (hence, length = time)

    if (before != nullptr)
        return std::max((int) time - (int) before->get_end_time(), 0);
    else
        return time;
}

TimelineUnit Project::get_chain_ahead(TimelineUnit time)
{
    auto curr = get_iter_at_time(time);
    if (curr == clip_vec.end())
        return 0;
    
    // Keep chaining clips together until a gap is found
    // (or the end of the vector is reached)

    TimelineUnit chain_length = (*curr)->get_end_time() - time;
    while (++curr != clip_vec.end())
    {
        // Gap is found when this clip doesn't start when the prev clip ends

        if ((*(curr - 1))->get_end_time() != (*curr)->get_start_time())
            break;
        else
            chain_length += (*curr)->get_length();
    }
    return chain_length;
}

void Project::save()
{
    // Must have name to save

    if (!name.has_value())
        throw ButterException("Cannot save nameless project");

    // No check exists here to prevent overwriting existing project
    // (since this is what saving a file does)
    // In the future, you can put a check when the file path is set
    // to prevent setting it to a filepath already in use
    // (or at least show a confirmation dialog)

    std::ofstream file {FileManager().get_data_path("projects/" + name.value() + ".proj")};

    // Write project parameters

    file << resolution.x << " " << resolution.y << " ";
    file << framerate << " ";

    // Timeline starts with a number specifying the number of clips
    // Then, each clip begins with its clip type (enum value), start position, and length
    // Each clip is then free to define its own serialization methods

    file << clip_vec.size() << " ";
    for (int i = 0; i < clip_vec.size(); ++i)
    {
        ClipData* clip = clip_vec.at(i).get();
        file << clip->get_clip_type() << " " << clip->get_start_time() << " " << clip->get_length() << " ";
        clip->save(file);
    }

    file.close();
}

void Project::export_video(std::filesystem::path filepath)
{
    // Accessing thread safe data, use a mutex
    // (Probably not necessary, but let's be safe)

    std::unique_lock<std::mutex> lock {basic_mutex, std::defer_lock};
    lock.lock();

    // Export task is stored persistently in this object
    // This, among other things, allows easy read/write access to
    // export state while it's running in a separate thread

    if (!export_finished)
        throw ButterException("Export task already in progress");

    // Reset export task

    export_finished = false;
    export_progress = 0;
    lock.unlock();

    // Open FFMPEG pipe
    // Most of these arguments are sourced from:
    // https://zulko.github.io/blog/2013/09/27/read-and-write-video-frames-in-python-using-ffmpeg/
    // (Fun fact: it appears Manim's FFMPEG implementation is also sourced from here)

    export_task.ffmpeg_pipe = subprocess::RunBuilder({
        "ffmpeg",
        "-y",
        "-f", "rawvideo",
        "-vcodec", "rawvideo",
        "-s", std::to_string(resolution.x) + "x" + std::to_string(resolution.y),
        "-r", std::to_string(framerate),
        "-pix_fmt", "rgb24",
        "-i", "pipe:",
        "-an",
        "-pix_fmt", "yuv420p",
        "-vcodec", "libx264",
        "-loglevel", "quiet",
        filepath.string()
    }).cin(subprocess::PipeOption::pipe).popen();

    export_task.buffer_size = 3 * resolution.x * resolution.y;
    export_task.buffer = new uint8_t[export_task.buffer_size];
    export_task.final_frame = std::max<TimelineUnit>(get_project_length(), 1);
    
    // Open thread using predefined function

    export_task.thread = std::thread(&Project::export_async, this);
    export_task.thread.detach();
}

// Used internally by `get_clip_at_time()`
// Exists as an intermediate function since having access to the iterator
// is useful for a lot of member functions to save time

std::vector<std::unique_ptr<ClipData>>::iterator Project::get_iter_at_time(TimelineUnit time)
{
    // Return early if:
    // a) empty timeline
    // b) negative time, or
    // c) every clip starts after the provided time

    if (clip_vec.empty())
        return clip_vec.end();
    if (time < 0)
        return clip_vec.end();
    if (time < clip_vec.at(0)->get_start_time())
        return clip_vec.end();

    // Attempt to find a clip that begins before the time
    // and ends after the time
    // Beginning is inclusive, end is exclusive

    for (auto it = clip_vec.begin(); it != clip_vec.end(); ++it)
    {
        if (time >= (*it)->get_start_time() && time < (*it)->get_end_time())
            return it;
    }
    return clip_vec.end();
}

void Project::proj_assert(bool condition, std::string fail_msg)
{
    if (!condition)
        throw ProjectLoadException(fail_msg);
}

void Project::write_frame_rgb24(TimelineUnit time, std::uint8_t* buffer)
{
    // For now, the entire frame is just one color

    ClipData* current_clip = get_clip_at_time(time);
    sf::Color color = (current_clip == nullptr) ? sf::Color::Black : (dynamic_cast<ColorClipData*>(current_clip)->get_color());

    // Buffer size = (width * height * 3) chars

    int w = resolution.x;
    int h = resolution.y;
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            buffer[3 * (x + y * w) + 0] = color.r;
            buffer[3 * (x + y * w) + 1] = color.g;
            buffer[3 * (x + y * w) + 2] = color.b;
        }
    }
}

void Project::export_async()
{
    // Write frames in specified format
    
    std::unique_lock<std::mutex> lock {basic_mutex, std::defer_lock};
    for (int f = 0; f < export_task.final_frame; ++f)
    {
        write_frame_rgb24(f, export_task.buffer);
        std::size_t result = subprocess::pipe_write(export_task.ffmpeg_pipe.cin, export_task.buffer, export_task.buffer_size);

        // Lock while writing to thread-safe members

        lock.lock();
        export_progress = (int) std::floor(f / ((float) export_task.final_frame) * 100.0);
        lock.unlock();
    }

    // Close pipe & conclude export

    lock.lock();
    export_finished = true;
    export_task.ffmpeg_pipe.close_cin();
    export_task.ffmpeg_pipe.close();
    delete[] export_task.buffer;
    lock.unlock();
}

bool Project::exists(std::string name)
{
    return std::filesystem::exists(FileManager().get_data_path("projects/" + name + ".proj"));
}
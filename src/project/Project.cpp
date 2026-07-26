#include "project/Project.h"

#include <subprocess.hpp>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "utility/core.h"
#include "utility/FileManager.h"
#include "utility/Logger.h"
#include "project/exceptions.h"

#include "project/timeline/ColorClip.h"

Project::Project()
{
    name = std::nullopt;

    // Use default project parameters

    framerate = 30;
    resolution = sf::Vector2u(400, 300);
}

Project::Project(std::string name)
{
    this->name = name;
    
    std::ifstream file {FileManager().get_data_path("projects/" + this->name.value() + ".proj")};
    proj_assert(file.is_open(), "Could not find project");

    // Load project parameters

    int int_buffer;
    file >> int_buffer;
    resolution.x = int_buffer;
    proj_assert(framerate > 0, "Invalid resolution");
    file >> int_buffer;
    resolution.y = int_buffer;
    proj_assert(framerate > 0, "Invalid resolution");
    file >> int_buffer;
    framerate = int_buffer;
    proj_assert(framerate > 0, "Invalid framerate");

    // Individually load each clip

    int clip_total;
    file >> clip_total;

    int clip_type;
    TimelineClip* new_clip = nullptr;
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
        switch (static_cast<TimelineClipType>(clip_type))
        {
        case TimelineClipType::COLOR:
            new_clip = new ColorClip(clip_start, clip_length, file);
        break;
        default:
            throw ButterException("Invalid clip type: " + std::to_string(clip_type));
        }
        timeline.push_back(new_clip);
    }

    file.close();
}

Project::~Project()
{
    // Clear memory

    for (auto clip : timeline)
        delete clip;
}

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

    ColorClip* new_clip = new ColorClip(start_time, length, color);
    auto next_clip = timeline.begin();
    while (next_clip < timeline.end())
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

    if (next_clip != timeline.begin())
    {
        auto prev_clip = next_clip - 1;
        if ((*prev_clip)->get_end_time() > start_time)
            return false;
    }

    // If all checks passed, insert clip
    
    timeline.insert(next_clip, new_clip);
    return true;
}

unsigned int Project::get_clip_total()
{
    return timeline.size();
}

TimelineClip* Project::get_clip_at_index(unsigned int index)
{
    try
    {
        return timeline.at(index);
    }
    catch (std::out_of_range e)
    {
        return nullptr;
    }
}

TimelineClip* Project::get_clip_at_time(TimelineUnit time)
{
    // Return early if:
    // a) empty timeline, or
    // b) every clip starts after the provided time

    if (timeline.empty())
        return nullptr;
    if (time < timeline.at(0)->get_start_time())
        return nullptr;

    // Attempt to find a clip that begins before the time and ends
    // after the time
    // Beginning is inclusive, end is exclusive

    for (TimelineClip* clip : timeline)
    {
        if (time >= clip->get_start_time() && time < clip->get_end_time())
            return clip;
    }
    return nullptr;
}

TimelineUnit Project::get_project_length()
{
    // Return the end time of the final clip
    // If the timeline is empty, return length of 1

    return (timeline.empty() ? 0 : timeline.back()->get_end_time());
}

std::string Project::get_project_length_approx()
{
    int time = (int) get_project_length();
    time = std::ceil(time / ((float) framerate));
    int sec = time % 60;
    time = (time - sec) / 60;
    int min = time % 60;
    time = (time - min) / 60;
    int hr = time;
    return (std::stringstream{} << std::setfill('0')
        << std::setw(2) << hr << "h "
        << std::setw(2) << min << "m "
        << std::setw(2) << sec << "s"
    ).str();
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

    file << timeline.size() << " ";
    for (int i = 0; i < timeline.size(); ++i)
    {
        TimelineClip* clip = timeline.at(i);
        file << clip->get_clip_type() << " " << clip->get_start_time() << " " << clip->get_length() << " ";
        clip->save(file);
    }

    file.close();
}

void Project::export_video(std::filesystem::path filepath)
{
    // Open FFMPEG pipe
    // Most of these arguments are sourced from:
    // https://zulko.github.io/blog/2013/09/27/read-and-write-video-frames-in-python-using-ffmpeg/
    // (Fun fact: it appears Manim's FFMPEG implementation is also sourced from here)

    subprocess::Popen ffmpeg_pipe = subprocess::RunBuilder({
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
        filepath.string()
    }).cin(subprocess::PipeOption::pipe).popen();

    // Write frames in specified format

    std::size_t buffer_size = 3 * resolution.x * resolution.y;
    std::uint8_t buffer[buffer_size];
    TimelineUnit last_frame = std::max<TimelineUnit>(get_project_length(), 1);
    for (int f = 0; f < last_frame; ++f)
    {
        write_frame_rgb24(f, buffer);
        std::size_t result = subprocess::pipe_write(ffmpeg_pipe.cin, buffer, buffer_size);
    }

    // Close pipe & conclude export

    ffmpeg_pipe.close_cin();
    ffmpeg_pipe.close();
}

bool Project::exists(std::string name)
{
    return std::filesystem::exists(FileManager().get_data_path("projects/" + name + ".proj"));
}

void Project::proj_assert(bool condition, std::string fail_msg)
{
    if (!condition)
        throw ProjectLoadException(fail_msg);
}

void Project::write_frame_rgb24(TimelineUnit time, std::uint8_t* buffer)
{
    // For now, the entire frame is just one color

    TimelineClip* current_clip = get_clip_at_time(time);
    sf::Color color = (current_clip == nullptr) ? sf::Color::Black : (dynamic_cast<ColorClip*>(current_clip)->get_color());

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
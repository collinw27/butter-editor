#include "project/Project.h"

#include "utility/core.h"
#include "utility/FileManager.h"
#include <fstream>

#include "project/timeline/EmptyClip.h"
#include "project/timeline/ColorClip.h"

Project::Project()
{
    name.reset();
}

Project::Project(std::string name)
{
    this->name = name;
    
    std::ifstream file {FileManager().get_data_path("projects/" + this->name.value() + ".proj")};
    if (!file.is_open())
        throw ButterException("Could not find project");

    // Individually load each clip

    int clip_total;
    file >> clip_total;

    int clip_type;
    TimelineClip* new_clip = nullptr;
    int clip_start;

    for (int i = 0; i < clip_total; ++i)
    {
        // Switch statement here is a little ugly
        // std::function can be used to make this more compact but less optimized,
        // so in this case I'll opt to use boilerplate for more efficiency

        file >> clip_type;
        file >> clip_start;
        switch (static_cast<TimelineClipType>(clip_type))
        {
        case TimelineClipType::EMPTY:
            new_clip = new EmptyClip(clip_start);
        break;
        case TimelineClipType::COLOR:
            new_clip = new ColorClip(clip_start, file);
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


void Project::add_color_clip(TimelinePos start_time, TimelinePos length, sf::Color color)
{
    // Add the color clip to the timeline
    // Find the first position it can slot in before something
    // If this doesn't exist, insert it at the end

    // TODO: Logic for cutting into other clips

    ColorClip* new_clip = new ColorClip(start_time, color);
    auto clip = timeline.begin();
    while (clip < timeline.end())
    {
        if (start_time < (*clip)->get_start_time())
        {
            timeline.insert(clip, new_clip);
            break;
        }
        ++clip;
    }
    if (clip == timeline.end())
        timeline.push_back(new_clip);
}

unsigned Project::get_clip_total()
{
    return timeline.size();
}

TimelineClip* Project::get_clip_at_index(unsigned index)
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

TimelineClip* Project::get_clip_at_time(TimelinePos time)
{
    // Return early if:
    // a) empty timeline, or
    // b) every clip starts after the provided time

    if (timeline.empty())
        return nullptr;
    if (time < timeline.at(0)->get_start_time())
        return nullptr;
    
    // Return the clip before the first clip that begins after the provided time
    // This works even in the case where no clip satisfies this condition,
    // since it will simply return the last clip (immediately before the end() iterator)
    // It's worth noting that the final clip extends infinitely, so it's impossible for the
    // provided time to be after every clip (in practice, the end clip should be an EmptyClip)

    auto clip = timeline.begin();
    while (clip < timeline.end())
    {
        if (time < (*clip)->get_start_time())
            break;
        ++clip;
    }
    return *(clip - 1);
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

    // Timeline starts with a number specifying the number of clips
    // Then, each clip begins with its clip type (enum value) and start position
    // Each clip is then free to define its own serialization methods

    file << timeline.size() << " ";
    for (int i = 0; i < timeline.size(); ++i)
    {
        TimelineClip* clip = timeline.at(i);
        file << clip->get_clip_type() << " " << clip->get_start_time() << " ";
        clip->save(file);
    }

    file.close();
}

bool Project::exists(std::string name)
{
    return std::filesystem::exists(FileManager().get_data_path("projects/" + name + ".proj"));
}
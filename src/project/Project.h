#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <mutex>
#include <filesystem>

#include <SFML/Graphics.hpp>
#include <subprocess.hpp>
#include "utility/core.h"
#include "project/clip/Clip.h"
#include "project/media/MediaItem.h"

// All project loading logic is within this class instead of FileManager
// This does require duplicating some logic, but this strategy is much quicker
// since the load function can be tailored to the class structure itself,
// and it doesn't require as many intermediate steps
// Efficiency is important here because project files can be arbitrarily large

// Here's my current philosophy about multithreading (might be long):
// Project exporting (and probably a some other frame-rendering operations in the future) is achieved through multithreading.
// This allows the program to keep responding during a rendering operation, especially in the case that an individual frame
// is expensive to render. However, with multithreading comes race conditions and the need to wrap a mutex around any data that
// might be concurrently accessed by two different threads. Since rendering a frame requires access to basically all project
// data, this would require all project operations to be wrapped in mutexes. Aside from being tedious and error-prone, this
// doesn't do anything to solve another problem: we need some way to stop the user from modifying the project while exporting
// is taking place. This could also be done with a manual check in each function, but consider the following alternative:
// What if we allow the exporting operation to "lock" the entire class, and take ownership of it throughout the process?
// While it's "locked", any operations on the project cannot be called by an external class. This prevents modifying the class
// while exporting, and it also sidesteps the need for mutexes by ensuring the exporting thread is the only thread that can
// access the data during its runtime. It's fine if you need to expose a few read-only methods that can still be called while
// locked, under the condition that any read/write operations to said data are always atomic, regardless of what thread is
// executing.
// So, how is this "lock" implemented in practice? My goal is that any project state that is susceptible to locking is
// contained within its own class. This is the way I've decided to do this:
// LockedProject - Base class, any data/methods within this class are thread-safe
// Project - Child of LockedProject, contains the bulk of project data/methods, members are not necessarily thread-safe
// since this portion of the project can only be accessed while unlocked
// The Editor class then has two different variables:
// `locked_project`: Always contains a pointer to the LockedProject* portion of the Project
// `project`: Contains a full-on Project* pointer, but only when unlocked (nullptr otherwise)

// Another note (since this essay wasn't long enough already):
// This class is designed such that it doesn't have to be aware of any other editor constructs
// ex. it has no knowledge of the timeline interface
// Because of this, modules that modify project data are responsible for reading from this class
// and keeping their own data in sync with the underlying project (since they will often store redundant
// data in a way that makes modifying the project easier)
// Whenever an external class modifies the project data, it should call one of this class' update methods
// to keep it in sync with the new update

class Editor;

struct ExportTask
{
    subprocess::Popen ffmpeg_pipe;
    std::thread thread;
    std::size_t buffer_size;
    std::uint8_t* buffer;
    VideoTime final_frame;
};

class LockedProject
{
protected:

    Editor& editor;

    // Thread-safe export information

    bool export_finished = true;
    int export_progress = 0;
    std::mutex basic_mutex;

    // This class should not be instantiated by itself
    // Therefore: protected constructor
    
    LockedProject(Editor& editor);

public:

    virtual ~LockedProject();
    
    bool is_exporting();
    int get_export_percentage();
};

class Project : public LockedProject
{
    // Basic info

    std::optional<std::string> name;
    int framerate;
    sf::Vector2u resolution;

    // State

    std::vector<std::unique_ptr<MediaItem>> media_vec;
    std::unordered_map<id_s, MediaItem*> media_map;
    std::vector<std::unique_ptr<Clip>> clip_vec;
    std::unordered_map<id_s, Clip*> clip_map;
    ExportTask export_task;

    // IDs

    id_s next_clip_id = ID_START;
    id_s next_media_id = ID_START;

public:

    Project(Editor& editor);
    Project(Editor& editor, std::string name);
    virtual ~Project();

    // Basic info

    bool named();
    std::string get_name();
    void set_name(std::string name);
    int get_framerate();
    sf::Vector2u get_resolution();

    // Media manipulation/reading

    id_s add_color_media(std::string display_name, sf::Color color);
    size_t get_media_total();
    id_s get_media_at_index(size_t index);
    std::string get_media_name(id_s media_id);
    sf::Color get_media_color(id_s media_id);

    // Timeline manipulation

    id_s add_color_clip(VideoTime start_time, VideoTime length, sf::Color color);
    void set_clip_start(id_s clip_id, VideoTime start);
    void set_clip_end(id_s clip_id, VideoTime end);
    void delete_clip(id_s clip_id);

    // Timeline reading

    size_t get_clip_total();
    id_s get_clip_at_index(size_t index);
    id_s get_clip_at_time(VideoTime time);
    VideoTime get_project_length();
    std::string get_project_length_approx();
    std::string to_string(VideoTime time);
    VideoTime get_gap_ahead(VideoTime time);
    VideoTime get_gap_behind(VideoTime time);
    VideoTime get_chain_ahead(VideoTime time);

    // Clip information

    VideoTime get_clip_start(id_s clip_id);
    VideoTime get_clip_length(id_s clip_id);
    VideoTime get_clip_end(id_s clip_id);
    sf::Color get_clip_color(id_s clip_id);

    // Output
    
    void save();
    void export_video(std::filesystem::path filepath);

    // Misc
    
    static bool exists(std::string name);

private:

    std::vector<std::unique_ptr<MediaItem>>::iterator get_media_iter(id_s media_id);
    std::vector<std::unique_ptr<Clip>>::iterator get_iter_from_id(id_s clip_id);
    std::vector<std::unique_ptr<Clip>>::iterator get_iter_at_time(VideoTime time);

    void proj_assert(bool condition, std::string fail_msg);
    void write_frame_rgb24(VideoTime time, std::uint8_t* buffer);
    void export_async();
};

#endif
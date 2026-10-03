#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <mutex>
#include <memory>
#include <filesystem>

#include <SFML/Graphics.hpp>
#include <subprocess.hpp>
#include "utility/core.h"
#include "project/types.h"
#include "project/clip/Clip.h"
#include "project/media/MediaItem.h"

#include "graphics/texture/GLTexture.h"
#include "graphics/GLFrameBuffer.h"

// All project loading logic is within this class instead of FileManager
// This does require duplicating some logic, but this strategy is much quicker
// since the load function can be tailored to the class structure itself,
// and it doesn't require as many intermediate steps
// Efficiency is important here because project files can be arbitrarily large

// Here's my current philosophy about multithreading (might be long):
// Project exporting (but NOT normal preview rendering) is achieved through multithreading. This allows the program to
// keep responding during a rendering operation, especially in the case that an individual frame is expensive to render.
// However, with multithreading comes race conditions and the need to wrap a mutex around any data that might be
// concurrently accessed by two different threads. Since rendering a frame requires access to basically all project data,
// this would require all project operations to be wrapped in mutexes. Aside from being tedious and error-prone, this
// doesn't do anything to solve another problem: we need some way to stop the user from modifying the project while
// exporting is taking place. This could also be done with a manual check in each function, but consider the following
// alternative: What if we allow the exporting operation to "lock" the entire class, which makes its data read-only?
// You can still have a handful of fields/methods that are modifiable, but these need to be wrapped in mutexes.
// To make this work, any methods that modify project state cannot be called while locked, even from the export thread
// (since modifying the project isn't necessary for exporting, only export-related variables are modified). As a useful
// shorthand, any methods that require writing and therefore cannot be called while "locked" are capitalized.

// Another note (since this essay wasn't long enough already):
// This class is designed such that it doesn't have to be aware of any other editor constructs
// ex. it has no knowledge of the timeline interface
// To keep the other modules in sync with its information, it emits a notification to the Editor class that the
// other modules can listen for and update themselves accordingly. Thus, for an external module modifying project
// data, the chain of causality works like this:
// Module needs to update -> Module updates project -> Project emits notification
//   -> Module receives notification -> Module updates its internal state accordingly

class Editor;

struct ExportTask
{
    subprocess::Popen ffmpeg_pipe;
    std::thread thread;
    std::size_t buffer_size;
    VideoTime final_frame;
    std::uint8_t* export_buffer;
    std::unique_ptr<GLFrameBuffer> render_buffer;
    std::vector<id_s> current_clips;
};

class Project
{
    Editor& editor;

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

    // Thread-safe export information
    // Read only can be used to prevent modifications during export
    // while multiple threads are accessing the project's data

    bool is_locked = false;
    bool export_finished = true;
    bool export_should_terminate = false;
    int export_progress = 0;
    std::mutex basic_mutex;

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
    void Set_name(std::string name);
    int get_framerate();
    sf::Vector2u get_resolution();

    // Media manipulation/reading

    size_t get_media_total();
    id_s get_media_at_index(size_t index);
    std::string get_media_name(id_s media_id);
    const GLTexture* get_media_thumbnail(id_s media_id);
    MediaType get_media_type(id_s media_id);
    id_s Add_color_media(std::string display_name, sf::Color color);
    id_s Add_image_media(std::string display_name, std::string filepath);

    // Timeline reading

    size_t get_clip_total();
    id_s get_clip_at_index(size_t index);
    id_s get_clip_at_time(VideoTime time);
    const GLTexture* get_clip_thumbnail(id_s clip_id);
    VideoTime get_project_length();
    std::string get_project_length_approx();
    std::string to_string(VideoTime time);
    VideoTime get_gap_ahead(VideoTime time);
    VideoTime get_gap_behind(VideoTime time);
    VideoTime get_chain_ahead(VideoTime time);

    // Timeline manipulation

    id_s Add_color_clip(VideoTime start_time, VideoTime length, sf::Color color);
    id_s Add_color_clip(VideoTime start_time, VideoTime length, id_s media_id);
    id_s Add_image_clip(VideoTime start_time, VideoTime length, id_s media_id);
    void Set_clip_start(id_s clip_id, VideoTime start);
    void Set_clip_end(id_s clip_id, VideoTime end);
    void Delete_clip(id_s clip_id);

    // Clip information

    VideoTime get_clip_start(id_s clip_id);
    VideoTime get_clip_length(id_s clip_id);
    VideoTime get_clip_end(id_s clip_id);
    sf::Color get_clip_bg_color(id_s clip_id);

    // Concurrency
    
    bool locked();
    void lock();
    void unlock();

    // Output
    void save();
    void clip_enter_frame(id_s clip_id, GLFrameBuffer* buffer);
    void clip_exit_frame(id_s clip_id, GLFrameBuffer* buffer);
    void clip_update_frame(id_s clip_id, GLFrameBuffer* buffer, VideoTime time);
    void Export_video(std::filesystem::path filepath);
    void cancel_export();
    bool is_exporting();
    int get_export_percentage();

    // Misc
    
    static bool exists(std::string name);
    static std::string read_string(std::ifstream& file);
    static void write_string(std::ofstream& file, const std::string& str);

private:

    std::vector<std::unique_ptr<MediaItem>>::iterator get_media_iter(id_s media_id);
    std::vector<std::unique_ptr<Clip>>::iterator get_iter_from_id(id_s clip_id);
    std::vector<std::unique_ptr<Clip>>::iterator get_iter_at_time(VideoTime time);

    id_s Add_generic_clip(VideoTime start_time, VideoTime length, Clip* new_clip);

    void proj_assert(bool condition, std::string fail_msg);
    void assert_unlocked();
    void write_frame_rgb24(VideoTime time, std::uint8_t* buffer);
    void export_async();
};

#endif
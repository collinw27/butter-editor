#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <SFML/Graphics.hpp>

#include "utility/core.h"
#include "project/timeline/TimelineClip.h"

// All project loading logic is within this class instead of FileManager
// This does require duplicating some logic, but this strategy is much quicker
// since the load function can be tailored to the class structure itself,
// and it doesn't require as many intermediate steps
// Efficiency is important here because project files can be arbitrarily large

class Project
{
    std::optional<std::string> name; 
    std::vector<TimelineClip*> timeline;

    int framerate;
    sf::Vector2u resolution;

public:

    Project();
    Project(std::string name);
    ~Project();

    bool named();
    std::string get_name();
    void set_name(std::string name);

    int get_framerate();
    sf::Vector2u get_resolution();

    bool add_color_clip(TimelineUnit start_time, TimelineUnit length, sf::Color color);
    unsigned int get_clip_total();
    TimelineClip* get_clip_at_index(unsigned int index);
    TimelineClip* get_clip_at_time(TimelineUnit time);
    TimelineUnit get_project_length();
    std::string get_project_length_approx();
    
    void save();
    void export_video(std::filesystem::path filepath);
    
    static bool exists(std::string name);

private:

    void proj_assert(bool condition, std::string fail_msg);
    void write_frame_rgb24(TimelineUnit time, std::uint8_t* buffer);
};

#endif
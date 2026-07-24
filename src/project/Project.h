#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include <vector>
#include <optional>
#include <filesystem>

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

public:

    Project();
    Project(std::string name);
    ~Project();

    bool named();
    std::string get_name();
    void set_name(std::string name);

    bool add_color_clip(TimelineUnit start_time, TimelineUnit length, sf::Color color);
    unsigned int get_clip_total();
    TimelineClip* get_clip_at_index(unsigned int index);
    TimelineClip* get_clip_at_time(TimelineUnit time);

    void save();

    static bool exists(std::string name);
};

#endif
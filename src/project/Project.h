#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include <optional>
#include <filesystem>

// All project loading logic is within this class instead of FileManager
// This does require duplicating some logic, but this strategy provides much more
// speed since the load function can be tailored to the class structure itself
// Efficiency is important here because project files can be arbitrarily large

class Project
{
    std::optional<std::string> name; 
    int test_number;

public:

    Project();
    Project(std::string name);

    bool named();
    std::string get_name();
    void set_name(std::string name);

    void save();

    static bool exists(std::string name);
};

#endif
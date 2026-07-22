#include "Project.h"

#include "utility/core.h"
#include "utility/FileManager.h"
#include <fstream>

Project::Project()
{
    name.reset();
    test_number = 0;
}

Project::Project(std::string name)
{
    this->name = name;
    
    std::ifstream file {FileManager().get_data_path("projects/" + this->name.value() + ".proj")};
    if (!file.is_open())
        throw ButterException("Could not find project");
    file >> test_number;
}

bool Project::check_named()
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
    ++test_number;
    file << test_number;
    file.close();
}

bool Project::exists(std::string name)
{
    return std::filesystem::exists(FileManager().get_data_path("projects/" + name + ".proj"));
}
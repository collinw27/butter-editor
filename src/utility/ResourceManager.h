#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <string>

class ResourceManager
{
    static ResourceManager *singleton_object;

    std::filesystem::path root_dir;
    sf::Font main_font;

public:

    static ResourceManager *singleton();
    ResourceManager();
    ~ResourceManager();

    std::string get_path(const std::string &path);
    std::string get_path(const std::filesystem::path &path);

    const sf::Font &get_font();
};

#endif
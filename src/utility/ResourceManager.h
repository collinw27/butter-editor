#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <string>

class ResourceManagerSingleton;

ResourceManagerSingleton& ResourceManager();

class ResourceManagerSingleton
{
    static ResourceManagerSingleton* singleton_object;

    std::filesystem::path root_dir;
    sf::Font main_font;
    sf::Font mono_font;

public:

    ResourceManagerSingleton();
    ~ResourceManagerSingleton();

    std::string get_path(const std::string& path);
    std::string get_path(const std::filesystem::path& path);
    const sf::Font& get_font();
    const sf::Font& get_mono();

    friend ResourceManagerSingleton& ResourceManager();
};

#endif
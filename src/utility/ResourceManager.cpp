#include "utility/ResourceManager.h"

#include <fstream>

#include "utility/Exceptions.h"

ResourceManager* ResourceManager::singleton_object = nullptr;

ResourceManager* ResourceManager::singleton()
{
    return singleton_object;
}

ResourceManager::ResourceManager()
{
    ResourceManager::singleton_object = this;
    
    std::ifstream file{"respath.txt"};
    if (!file.is_open())
    {
        throw ButterException("Could not locate respath.txt!");
    }

    std::string filepath;
    std::getline(file, filepath);
    root_dir = filepath;

    if (!main_font.openFromFile(root_dir/"font/Arial.ttf"))
    {
        throw ButterException("Could not locate Arial.ttf!");
    }
    if (!mono_font.openFromFile(root_dir/"font/Consolas.ttf"))
    {
        throw ButterException("Could not locate Consolas.ttf!");
    }
}

ResourceManager::~ResourceManager()
{
    ResourceManager::singleton_object = nullptr;
}

std::string ResourceManager::get_path(const std::string& path = "")
{
    return (root_dir/path).string();
}

std::string ResourceManager::get_path(const std::filesystem::path& path)
{
    return (root_dir/path).string();
}

const sf::Font& ResourceManager::get_font()
{
    return main_font;
}

const sf::Font& ResourceManager::get_mono()
{
    return mono_font;
}
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <string>
#include <utility> // for std::pair
#include "utility/UserSettings.h"

class FileManagerSingleton;

FileManagerSingleton& FileManager();

class FileManagerSingleton
{
    static FileManagerSingleton* singleton_object;

    std::filesystem::path res_root_dir;
    std::filesystem::path data_root_dir;    

    sf::Font main_font;
    sf::Font mono_font;

    UserSettings user_settings;

public:

    FileManagerSingleton();
    ~FileManagerSingleton();

    std::string get_res_path(const std::filesystem::path& path);
    std::string get_data_path(const std::filesystem::path& path);

    const UserSettings& get_user_settings();
    void update_user_settings(const UserSettings& settings);

    std::string load_shader(std::string filename);

    friend FileManagerSingleton& FileManager();

private:

    void load_user_settings();
    std::pair<std::string, std::string> file_read_data_line(std::ifstream& file);
    int32_t file_read_int(std::string input, int32_t min_val = INT32_MIN, int32_t max_val = INT32_MAX);
};

#endif
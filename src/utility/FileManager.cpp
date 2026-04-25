#include "utility/FileManager.h"

#include <fstream>

#include "utility/core.h"

FileManagerSingleton* FileManagerSingleton::singleton_object = nullptr;

FileManagerSingleton& FileManager()
{
    return *FileManagerSingleton::singleton_object;
}

FileManagerSingleton::FileManagerSingleton()
{
    if (FileManagerSingleton::singleton_object != nullptr)
        throw ButterException("Reinitialization of singleton");
    FileManagerSingleton::singleton_object = this;
    
    std::ifstream res_file{"respath.txt"};
    if (!res_file.is_open())
        throw ButterException("Could not locate respath.txt!");
    std::string res_filepath;
    std::getline(res_file, res_filepath);
    res_root_dir = res_filepath;
    
    std::ifstream data_file{"datapath.txt"};
    if (!data_file.is_open())
        throw ButterException("Could not locate datapath.txt!");
    std::string data_filepath;
    std::getline(data_file, data_filepath);
    data_root_dir = data_filepath;

    if (!main_font.openFromFile(res_root_dir/"font/Lato-Regular.ttf"))
        throw ButterException("Could not locate Lato-Regular.ttf!");
    if (!mono_font.openFromFile(res_root_dir/"font/Consolas.ttf"))
        throw ButterException("Could not locate Consolas.ttf!");

    load_user_settings();
}

FileManagerSingleton::~FileManagerSingleton()
{
    FileManagerSingleton::singleton_object = nullptr;
}

std::string FileManagerSingleton::get_res_path(const std::filesystem::path& path)
{
    return (res_root_dir/path).string();
}

std::string FileManagerSingleton::get_data_path(const std::filesystem::path& path)
{
    return (data_root_dir/path).string();
}

// To modify, must pass a copy to `write_user_settings()`
// This is a more safe way of doing it (prevents modifying it without writing),
// but may become slow as the settings file becomes bigger

const UserSettings& FileManagerSingleton::get_user_settings()
{
    return user_settings;
}

void FileManagerSingleton::update_user_settings(const UserSettings& new_settings)
{
    user_settings = new_settings;

    // Create settings file

    std::ofstream file{get_data_path("user_settings.txt"), std::ios_base::out};
    file << "ui_scale_index: " << user_settings.ui_scale_index;
    file.close();
}

std::string FileManagerSingleton::load_shader(std::string filename)
{
    std::ifstream shader_file {res_root_dir/"shader"/filename};
    if (!shader_file.is_open())
        throw ButterException("Could not locate " + filename);
    return (std::stringstream{} << shader_file.rdbuf()).str();
}

void FileManagerSingleton::load_user_settings()
{
    // Default values are used if not provided

    user_settings = UserSettings{};

    // Attempt to find file
    // If it doesn't exist, that's fine, just silently abort

    std::ifstream file{get_data_path("user_settings.txt")};
    if (!file.is_open())
        return;

    // Sequentially read settings
    // Data validation is important!
    
    while (!file.eof())
    {
        auto line = file_read_data_line(file);
        if (line.first == "ui_scale_index")
        {
            user_settings.ui_scale_index = file_read_int(line.second, -5, 10);
        }
    }
}

std::pair<std::string, std::string> FileManagerSingleton::file_read_data_line(std::ifstream& file)
{
    // Must match format "name: some other data"

    std::pair<std::string, std::string> output;
    std::getline(file, output.first, ':');
    if (output.first.length() == 0)
        throw ButterException("Invalid file data (empty key)");
    std::getline(file, output.second);
    if (output.second.length() == 0)
        throw ButterException("Invalid file data (empty value)");
    return output;
}

int32_t FileManagerSingleton::file_read_int(std::string input, int32_t min_val, int32_t max_val)
{
    try
    {
        int32_t output = std::stoi(input);
        if (output < min_val || output > max_val)
            throw ButterException("Invalid file integer: out of range (" + std::to_string(output) + ")");
        return output;
    }
    catch (std::invalid_argument error)
    {
        throw ButterException("Invalid file integer");
    }
    catch (std::out_of_range error)
    {
        throw ButterException("Invalid file integer");
    }
}
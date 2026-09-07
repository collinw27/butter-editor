#include "project/media/ImageMedia.h"

#include "utility/FileManager.h"
#include "utility/Graphics.h"
#include "project/Project.h"

ImageMedia::ImageMedia(id_s id, std::string display_name, std::filesystem::path image_path)
    : MediaItem{id, display_name}
{
    this->image_path = image_path;
    if (std::filesystem::exists(image_path))
        this->image_tex.reset(new GLTexture(image_path));
    else
        this->image_tex.reset(new GLTexture(Graphics().error_image()));
}

ImageMedia::ImageMedia(id_s id, std::string display_name, std::ifstream& file)
    : MediaItem{id, display_name}
{
    image_path = Project::read_string(file);
    if (std::filesystem::exists(image_path))
        this->image_tex.reset(new GLTexture(image_path));
    else
        this->image_tex.reset(new GLTexture(Graphics().error_image()));
}

int ImageMedia::get_media_type()
{
    return (int) MediaType::IMAGE;
}

const GLTexture& ImageMedia::get_thumbnail()
{
    return *image_tex.get();
}

void ImageMedia::save(std::ofstream& file)
{
    Project::write_string(file, image_path.string());
}
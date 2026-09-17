#include "project/media/ImageMedia.h"

#include "utility/FileManager.h"
#include "utility/Graphics.h"
#include "project/types.h"

// Importing Project is not ideal, but also not a big problem
// since only static helper methods are used
// These methods should probably just be placed in another file

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

MediaType ImageMedia::get_media_type()
{
    return MediaType::IMAGE;
}

// Right now, `get_thumbnail()` and `get_texture()` function identically
// A separation is still maintained since they are conceptually different,
// and it's possible one will be changed in the future independently of the other

const GLTexture* ImageMedia::get_thumbnail()
{
    return image_tex.get();
}

const GLTexture* ImageMedia::get_texture()
{
    return image_tex.get();
}

void ImageMedia::save(std::ofstream& file)
{
    Project::write_string(file, image_path.string());
}
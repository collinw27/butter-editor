#include "editor/preview/PreviewModule.h"

#include <cmath>
#include "utility/FileManager.h"
#include "utility/Graphics.h"
#include "editor/Editor.h"
#include "editor/notifs.h"

PreviewModule::PreviewModule(Editor& editor)
    : EditorModule(editor)
{
    opt_into_notifs(NOTIF_TIMELINE::ID);
}

void PreviewModule::set_video_output(const GLBufferTexture* buffer)
{
    video_texture = buffer;
    video_output.reset(GLSprite::create(container.get(), video_texture, sf::Vector2f()));
    fit_video_output();
}

void PreviewModule::apply_bounds()
{
    fit_video_output();
}

void PreviewModule::fit_video_output()
{
    // The texture is scaled down and centered

    if (video_texture)
    {
        sf::Vector2f video_size = video_texture->get_size();
        if (video_texture->get_size().x > 0 && video_texture->get_size().y > 0)
        {
            float fit_x_scale = container->get_size().x / video_size.x;
            float fit_y_scale = container->get_size().y / video_size.y;
            video_output->set_scale(sf::Vector2f(1, 1) * std::min(fit_x_scale, fit_y_scale));
            sf::Vector2f scaled_size = video_size * std::min(fit_x_scale, fit_y_scale);
            video_output->set_position((container->get_size() - scaled_size) * 0.5f);
        }
    }
}
#include "MediaModule.h"

#include <cmath>
#include "utility/core.h"
#include "utility/Graphics.h"
#include "editor/Editor.h"
#include "editor/notifs.h"
#include "editor/media/DragMedia.h"

MediaModule::MediaModule(Editor& editor)
    : EditorModule{editor}
{
    highlight_rect = std::unique_ptr<GLRectangle>(GLRectangle::create(container.get()));
    highlight_rect->set_fill_color(Editor::C_HIGHLIGHT);
    highlight_rect->set_visible(false);
    highlight_rect->set_position(sf::Vector2f(0, 8));

    opt_into_notifs(NOTIF_MEDIA::ID);

    reload();
}

void MediaModule::reload()
{
    media_vec.clear();

    Project* project = editor.get_project();

    for (int i = 0; i < project->get_media_total(); ++i)
    {
        id_s media_id = project->get_media_at_index(i);
        GLRectangle* rect_node = GLRectangle::create(container.get(), sf::Vector2f(), sf::Vector2f(12, 12));
        sf::Color media_color = project->get_media_color(media_id);
        rect_node->set_fill_color(media_color);
        GLText* text_node = GLText::create(container.get(), Graphics().mono_font(), 0u, project->get_media_name(media_id));
        media_vec.push_back(MediaData{media_id, media_color, std::unique_ptr<GLRectangle>(rect_node), std::unique_ptr<GLText>(text_node)});
    }

    render_items();
}

void MediaModule::apply_bounds()
{
    highlight_rect->set_size(sf::Vector2f(bounds.size.x, unit_height * ui_scale));
    highlight_rect->set_visible(false);
}

void MediaModule::apply_ui_scale()
{
    unit_height = lerp_remap(0.6, 2.0, 21.2, 22.2, ui_scale);
    highlight_rect->set_size(sf::Vector2f(bounds.size.x, unit_height * ui_scale));
    highlight_rect->set_visible(false);
    render_items();
}

void MediaModule::on_mouse_press(sf::Vector2i position, bool focused, InputButton button)
{
    if (focused && button == InputButton::LEFT)
    {
        // There's a more efficient way to resolve this than using a for loop,
        // but it's fine for the time being

        for (int i = 0; i < media_vec.size(); ++i)
        {
            if (get_item_bounds(i).contains(position))
            {
                editor.set_drag_event(std::unique_ptr<DragMedia>(new DragMedia(media_vec.at(i).color)));
                break;
            }
        }
    }
}

void MediaModule::on_mouse_move(sf::Vector2i position, bool focused, DragMouseEvent* drag_event)
{
    highlight_rect->set_visible(false);
    if (focused)
    {
        for (int i = 0; i < media_vec.size(); ++i)
        {
            if (get_item_bounds(i).contains(position))
            {
                highlight_rect->set_visible(true);
                highlight_rect->set_position(sf::Vector2f(get_item_bounds(i).position));
                break;
            }
        }
    }
}

void MediaModule::on_notif(int notif_class, int notif_type, size_t num_args, void** arg_ptrs)
{
    if (notif_class == NOTIF_MEDIA::ID)
    {
        switch (notif_type)
        {
        case NOTIF_MEDIA::MEDIA_CREATED:
        {
            id_s media_id = *((id_s*) arg_ptrs[0]);
            Project* project = editor.get_project();
            GLRectangle* rect_node = GLRectangle::create(container.get(), sf::Vector2f(), sf::Vector2f(12, 12));
            sf::Color media_color = project->get_media_color(media_id);
            rect_node->set_fill_color(media_color);
            GLText* text_node = GLText::create(container.get(), Graphics().mono_font(), 0u, project->get_media_name(media_id));
            media_vec.push_back(MediaData{media_id, media_color, std::unique_ptr<GLRectangle>(rect_node), std::unique_ptr<GLText>(text_node)});
            
            render_items();
        }
        break;
        }
    }
}

void MediaModule::render_items()
{
    for (int i = 0; i < media_vec.size(); ++i)
    {
        MediaData& this_media = media_vec.at(i);
        this_media.color_node->set_position(sf::Vector2f(12, 10 + (int)(unit_height * ui_scale * i)));
        this_media.text_node->set_position(sf::Vector2f(12 + (int)(1.2 * (unit_height * ui_scale)), 10 + (int)(unit_height * ui_scale * i)));
        this_media.text_node->set_char_size((unsigned int)(16.f * ui_scale));
        this_media.color_node->set_size(sf::Vector2f(1, 1) * std::floor(unit_height * ui_scale - 4));
    }
}

sf::IntRect MediaModule::get_item_bounds(int index)
{
    return sf::IntRect({0, 8 + (int)(unit_height * ui_scale * index)}, {bounds.size.x, (int)(unit_height * ui_scale)});
}
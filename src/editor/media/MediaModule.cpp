#include "MediaModule.h"

#include <cmath>
#include "utility/core.h"
#include "utility/Graphics.h"
#include "editor/Editor.h"

MediaModule::MediaModule(Editor& editor)
    : EditorModule{editor}
{
    highlight_rect = std::unique_ptr<GLRectangle>(GLRectangle::create(container));
    highlight_rect->set_fill_color(Editor::C_HIGHLIGHT);
    highlight_rect->set_visible(false);
    highlight_rect->set_position(sf::Vector2f(0, 8));

    // Just use default placeholders for now

    media.push_back(MediaItem{sf::Color::Red, "Red"});
    media.push_back(MediaItem{sf::Color::Green, "Green"});
    media.push_back(MediaItem{sf::Color::Blue, "Blue"});
    for (MediaItem& this_media : media)
    {
        media_colors.push_back(std::unique_ptr<GLRectangle>(GLRectangle::create(container, sf::Vector2f(), sf::Vector2f(12, 12))));
        media_colors.back()->set_fill_color(this_media.color);
        media_text.push_back(std::unique_ptr<GLText>(GLText::create(container, Graphics().mono_font(), 10u, this_media.name)));
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
    for (int i = 0; i < media.size(); ++i)
    {
        media_text.at(i)->set_char_size((unsigned int)(16.f * ui_scale));
        media_colors.at(i)->set_size(sf::Vector2f(1, 1) * std::floor(unit_height * ui_scale - 4));
    }
    render_items();
}

void MediaModule::on_mouse_move(sf::Vector2i position, bool focused, DragMouse* drag_event)
{
    highlight_rect->set_visible(false);
    if (focused)
    {
        for (int i = 0; i < media.size(); ++i)
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

void MediaModule::render_items()
{
    for (int i = 0; i < media.size(); ++i)
    {
        MediaItem& this_media = media.at(i);
        media_colors.at(i)->set_position(sf::Vector2f(12, 10 + (int)(unit_height * ui_scale * i)));
        media_text.at(i)->set_position(sf::Vector2f(12 + (int)(1.2 * (unit_height * ui_scale)), 10 + (int)(unit_height * ui_scale * i)));
    }
}

sf::IntRect MediaModule::get_item_bounds(int index)
{
    return sf::IntRect({0, 8 + (int)(unit_height * ui_scale * index)}, {bounds.size.x, (int)(unit_height * ui_scale)});
}
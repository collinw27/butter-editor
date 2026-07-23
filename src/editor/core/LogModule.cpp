#include "editor/core/LogModule.h"

#include "utility/core.h"
#include "utility/Graphics.h"
#include "utility/Logger.h"
#include "editor/Editor.h"

constexpr int MAX_HISTORY = 8;

LogModule::LogModule(Editor& editor)
    : EditorModule(editor)
{
    highlight_rect = GLRectangle::create(container);
    highlight_rect->set_fill_color(Editor::C_HIGHLIGHT);
    highlight_rect->set_visible(false);
    highlight_rect->set_position(sf::Vector2f(0, 8));

    history_text = GLText::create(container, Graphics().mono_font(), 10u, "");
    history_text->set_position(sf::Vector2f(12, 8));
    error_text = GLText::create(container, Graphics().mono_font(), 10u, "");
    error_text->set_position(sf::Vector2f(12, 8));
    error_text->set_color(Editor::C_INVALID);

    render_text();
}

void LogModule::apply_bounds()
{
    highlight_rect->set_size(sf::Vector2f(bounds.size.x, unit_height * ui_scale));
    highlight_rect->set_visible(false);
}

void LogModule::apply_ui_scale()
{
    unit_height = lerp_remap(0.6, 2.0, 21.2, 22.2, ui_scale);
    highlight_rect->set_size(sf::Vector2f(bounds.size.x, unit_height * ui_scale));
    history_text->set_char_size((unsigned int)(16.f * ui_scale));
    error_text->set_char_size((unsigned int)(16.f * ui_scale));
    highlight_rect->set_visible(false);
}

void LogModule::push_command(std::string command)
{
    history.insert(history.begin(), command);
    while (history.size() > MAX_HISTORY)
        history.pop_back();
    error = std::nullopt;
    render_text();
}

void LogModule::push_error(std::string error)
{
    this->error = error;
    render_text();
}

void LogModule::on_mouse_moved(sf::Vector2f position, bool focused)
{
    highlight_rect->set_visible(false);
    if (focused)
    {
        int total = history.size() + (error.has_value() ? 1 : 0);
        for (int i = 0; i < total; ++i)
        {
            if (get_item_bounds(i).contains(sf::Vector2i(position)))
            {
                highlight_rect->set_visible(true);
                highlight_rect->set_position(sf::Vector2f(get_item_bounds(i).position));
                break;
            }
        }
    }
}

void LogModule::render_text()
{
    // Start with error if necessary

    error_text->set_string(error.has_value() ? error.value() : "");

    std::string text_string = error.has_value() ? "\n" : "";
    for (std::string line : history)
        text_string += line + "\n";
    history_text->set_string(text_string);
}

sf::IntRect LogModule::get_item_bounds(int index)
{
    return sf::IntRect({0, 8 + (int)(unit_height * ui_scale * index)}, {bounds.size.x, (int)(unit_height * ui_scale)});
}
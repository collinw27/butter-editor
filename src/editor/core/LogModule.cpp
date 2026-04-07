#include "editor/core/LogModule.h"

#include "utility/Graphics.h"
#include "editor/Editor.h"

constexpr int MAX_HISTORY = 8;

LogModule::LogModule(Editor& editor)
    : EditorModule(editor)
{
    highlight_rect = GLRectangle::create(container);
    highlight_rect->set_fill_color(Editor::C_HIGHLIGHT);

    history_text = GLText::create(container, Graphics().mono_font(), 10u, "");
    history_text->set_position(sf::Vector2f(12, 8));
    error_text = GLText::create(container, Graphics().mono_font(), 10u, "");
    error_text->set_position(sf::Vector2f(12, 8));
    error_text->set_color(Editor::C_INVALID);

    render_text();
}

void LogModule::set_bounds(const sf::IntRect& bounds)
{
    EditorModule::set_bounds(bounds);
}

void LogModule::set_ui_scale(float new_scale)
{
    EditorModule::set_ui_scale(new_scale);
    history_text->set_char_size((unsigned)(16.f * ui_scale));
    error_text->set_char_size((unsigned)(16.f * ui_scale));
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

void LogModule::render_text()
{
    // Start with error if necessary

    error_text->set_string(error.has_value() ? error.value() : "");

    std::string text_string = error.has_value() ? "\n" : "";
    for (std::string line : history)
        text_string += line + "\n";
    history_text->set_string(text_string);
}
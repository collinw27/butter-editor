#ifndef LOG_MODULE_H
#define LOG_MODULE_H

#include "editor/EditorModule.h"

#include <string>
#include <vector>
#include <optional>
#include "graphics/nodes.h"

class LogModule : public EditorModule
{
    GLText* history_text;
    GLText* error_text;
    GLRectangle* highlight_rect;

    // Draws the last-executed commands
    // Shows the error message at the top, if applicable
    // The error message is overwritten on the next executed command

    std::vector<std::string> history {};
    std::optional<std::string> error = std::nullopt;

public:

    LogModule(Editor& editor);

    virtual void set_bounds(const sf::IntRect& bounds) override;
    virtual void set_ui_scale(float new_scale) override;

    void push_command(std::string command);
    void push_error(std::string error);

private:

    void render_text();
    // sf::IntRect get_item_bounds(int index);
};

#endif
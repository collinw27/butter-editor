#ifndef LOG_MODULE_H
#define LOG_MODULE_H

#include "editor/core/EditorModule.h"

#include <string>
#include <vector>
#include <optional>
#include "graphics/nodes.h"

class LogModule : public EditorModule
{
    std::unique_ptr<GLText> history_text;
    std::unique_ptr<GLText> error_text;
    std::unique_ptr<GLRectangle> highlight_rect;
    float unit_height;

    // Draws the last-executed commands
    // Shows the error message at the top, if applicable
    // The error message is overwritten on the next executed command

    std::vector<std::string> history {};
    std::optional<std::string> error = std::nullopt;

public:

    LogModule(Editor& editor);

    virtual void apply_bounds() override;
    virtual void apply_ui_scale() override;

    void push_command(std::string command);
    void push_error(std::string error);
    
    virtual void on_mouse_move(sf::Vector2i position, bool focused, DragMouseEvent* drag_event) override;

private:

    void render_text();
    sf::IntRect get_item_bounds(int index);
};

#endif
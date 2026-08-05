#ifndef MEDIA_MODULE_H
#define MEDIA_MODULE_H

#include <string>
#include <vector>
#include <optional>
#include <memory>

#include <SFML/Graphics.hpp>
#include "editor/core/EditorModule.h"
#include "utility/Input.h"
#include "graphics/nodes.h"

struct MediaItem
{
    sf::Color color;
    std::string name;
};

class MediaModule : public EditorModule
{
    std::vector<MediaItem> media;
    std::vector<std::unique_ptr<GLRectangle>> media_colors;
    std::vector<std::unique_ptr<GLText>> media_text;
    std::unique_ptr<GLRectangle> highlight_rect;
    float unit_height;

public:

    MediaModule(Editor& editor);

    virtual void apply_bounds() override;
    virtual void apply_ui_scale() override;
    
    virtual void on_mouse_press(sf::Vector2i position, bool focused, InputButton button) override;
    virtual void on_mouse_move(sf::Vector2i position, bool focused, DragMouse* drag_event) override;

private:

    void render_items();
    sf::IntRect get_item_bounds(int index);
};

#endif
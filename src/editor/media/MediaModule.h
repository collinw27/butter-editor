#ifndef MEDIA_MODULE_H
#define MEDIA_MODULE_H

#include <string>
#include <vector>
#include <optional>
#include <memory>

#include <SFML/Graphics.hpp>
#include "editor/core/EditorModule.h"
#include "utility/core.h"
#include "utility/Input.h"
#include "graphics/nodes.h"

struct MediaData
{
    id_s id;
    sf::Color color;
    std::unique_ptr<GLRectangle> color_node;
    std::unique_ptr<GLText> text_node;
};

class MediaModule : public EditorModule
{
    std::vector<MediaData> media_vec;
    std::unique_ptr<GLRectangle> highlight_rect;
    float unit_height;

public:

    MediaModule(Editor& editor);
    void reload();

    virtual void apply_bounds() override;
    virtual void apply_ui_scale() override;
    
    virtual void on_mouse_press(sf::Vector2i position, bool focused, InputButton button) override;
    virtual void on_mouse_move(sf::Vector2i position, bool focused, DragMouseEvent* drag_event) override;

    virtual void on_notif(int notif_class, int notif_type, size_t num_args, void** arg_ptrs) override;

private:

    void render_items();
    sf::IntRect get_item_bounds(int index);
};

#endif
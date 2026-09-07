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
#include "project/Project.h"

struct MediaData
{
    id_s id;
    std::unique_ptr<GLTexture> thumbnail_tex;
    std::unique_ptr<GLSprite> thumbnail_node;
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

    void add_item(Project* project, id_s media_id);
    void render_items();
    sf::IntRect get_item_bounds(int index);
};

#endif
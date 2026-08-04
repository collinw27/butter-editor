#ifndef TIMELINE_MODULE
#define TIMELINE_MODULE

#include "editor/core/EditorModule.h"
#include "editor/core/DragMouse.h"
#include "graphics/nodes.h"
#include "editor/timeline/clip/Clip.h"

class TimelineModule : public EditorModule
{
    float scroll_pct;
    int scroll_max;
    int scroll_span;
    int zoom_factor;
    
    std::unique_ptr<GLNode> clips_anchor;
    std::unique_ptr<GLNode> clips_scaler;

    // The clip display is layered to display properly
    // Selection outlines should display above non-selected clips,
    // but are still behind all selected clips

    std::unique_ptr<GLNode> outline_layer;
    std::unique_ptr<GLNode> clip_layer;
    std::vector<std::unique_ptr<Clip>> clips;
    std::vector<Clip*> selected_clips;
    Clip* hovered_clip = nullptr;

    std::unique_ptr<GLRectangle> scroll_bar;

public:

    TimelineModule(Editor& editor);

    virtual void apply_bounds() override;
    virtual void apply_ui_scale() override;

    void scroll_left();
    void scroll_right();
    void zoom_in();
    void zoom_out();
    void refresh_clips();

    void select_all();
    void deselect_all();
    
    virtual void on_mouse_press(sf::Vector2i position, bool focused) override;
    virtual void on_mouse_move(sf::Vector2i position, bool focused, DragMouse* drag_event) override;
    virtual void on_mouse_release(sf::Vector2i position, bool focused, DragMouse* drag_event) override;
    virtual void on_mouse_drop(sf::Vector2i position, DragMouse* drag_event) override;

private:

    bool is_position_in_scroll(sf::Vector2i relative_pos);
    float x_to_time(int x);
    
    void update_scroll();
    void update_zoom();
    void update_scroll_color(bool hovering, bool dragging);
};

#endif
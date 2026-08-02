#ifndef TIMELINE_MODULE
#define TIMELINE_MODULE

#include "editor/core/EditorModule.h"
#include "editor/core/DragMouse.h"

#include "graphics/nodes.h"

class TimelineModule : public EditorModule
{
    float scroll_pct;
    int scroll_max;
    int scroll_span;
    
    GLNode* clips_anchor;
    std::vector<GLRectangle*> clips;
    GLRectangle* scroll_bar;

public:

    TimelineModule(Editor& editor);
    ~TimelineModule();

    virtual void apply_bounds() override;
    virtual void apply_ui_scale() override;

    void refresh_clips();
    
    virtual void on_mouse_press(sf::Vector2i position, bool focused) override;
    virtual void on_mouse_move(sf::Vector2i position, bool focused, DragMouse* drag_event) override;
    virtual void on_mouse_release(sf::Vector2i position, bool focused, DragMouse* drag_event) override;
    virtual void on_mouse_drop(sf::Vector2i position, DragMouse* drag_event) override;

private:

    bool is_position_in_scroll(sf::Vector2i relative_pos);
    void update_scroll();
    void update_scroll_color(bool hovering, bool dragging);
};

#endif
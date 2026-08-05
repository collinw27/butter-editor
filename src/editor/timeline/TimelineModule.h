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
    float scroll_amount;
    float zoom_amount;
    
    std::unique_ptr<GLNode> clips_anchor;
    std::unique_ptr<GLNode> clips_scaler;

    // The clip display is layered to display properly
    // Selection outlines should display above non-selected clips,
    // but are still behind all selected clips

    std::unique_ptr<GLNode> clip_layer;
    std::unique_ptr<GLNode> outline_layer;
    std::unique_ptr<GLNode> selection_layer;

    // Dynamically-allocated clips are stored sequentially in a vector
    // Other Clip-referencing variables are able to store pointers to these clips
    // Care must be taken to ensure dangling pointers are not created when clips
    // are deleted, projects are switched, etc.
    // To prevent accidental mishaps, access to this memory is forced to use
    // ClipMemoryManager, which makes these operations much safer

    class ClipMemoryManager
    {
        std::vector<Clip*> clips;
        std::vector<Clip*> selected_clips;
        Clip* hovered_clip = nullptr;

    public:

        ~ClipMemoryManager();

        void add_clip(Clip* clip);
        void remove_clip(Clip* clip);
        void clear_all();
        const std::vector<Clip*>& get_clips();

        void select_clip(Clip* clip);
        void deselect_clip(Clip* clip);
        const std::vector<Clip*>& get_selected_clips();

        void set_hovered_clip(Clip* clip);
        Clip* get_hovered_clip();
    };
    ClipMemoryManager clip_mem;

    // Other nodes

    std::unique_ptr<GLRectangle> scroll_bar;

    // Not storing the project here is a deliberate choice
    // The Editor class needs to be able to nullify the Project pointer
    // whenever the Project is locked

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
    
    virtual void on_update() override;
    virtual void on_mouse_press(sf::Vector2i position, bool focused, InputButton button) override;
    virtual void on_mouse_move(sf::Vector2i position, bool focused, DragMouse* drag_event) override;
    virtual void on_mouse_release(sf::Vector2i position, bool focused, InputButton button, DragMouse* drag_event) override;
    virtual void on_mouse_drop(sf::Vector2i position, DragMouse* drag_event) override;

private:

    bool is_position_in_scroll(sf::Vector2i relative_pos);
    float x_to_time(int x);

    void select_clip(Clip* clip);
    void deselect_clip(Clip* clip);
    void delete_clip(Clip* clip);
    
    void update_scroll();
    void update_zoom();
    void update_scroll_color(bool hovering, bool dragging);
};

#endif
#ifndef TIMELINE_MODULE
#define TIMELINE_MODULE

#include <tuple>
#include "editor/core/EditorModule.h"
#include "editor/core/mouse/DragMouseEvent.h"
#include "graphics/nodes.h"
#include "editor/timeline/Clip.h"

class TimelineModule : public EditorModule
{
    float scroll_pct;
    VideoTime scroll_max;
    int scroll_span;
    int zoom_factor;
    float scroll_amount;
    float zoom_amount;
    int clips_y;
    int playhead_time = 0;
    
    std::unique_ptr<GLNode> clips_anchor;
    std::unique_ptr<GLNode> clips_scaler;

    // The clip display is layered to display properly
    // Selection outlines should display above non-selected clips,
    // but are still behind all selected clips

    std::unique_ptr<GLNode> clip_layer;
    std::unique_ptr<GLNode> outline_layer;
    std::unique_ptr<GLNode> selection_layer;

    // Used for previewing dragging media into the editor

    std::unique_ptr<GLRectangle> ghost_clip;

    // The ruler will be given its own shader to show ruler lines eventually

    std::unique_ptr<GLRectangle> ruler;
    std::unique_ptr<GLOutlinedRectangle> padding_rect;
    std::unique_ptr<GLTexture> tex_playhead;
    std::unique_ptr<GLSprite> playhead_sprite;
    std::unique_ptr<GLRectangle> playhead_line;
    std::unique_ptr<GLNode> playhead;
    
    // Dynamically-allocated Clips are stored sequentially in a vector
    // Other Clip-referencing variables are able to store pointers to these clips
    // Care must be taken to ensure dangling pointers are not created when clips
    // are deleted, projects are switched, etc.
    // To prevent accidental mishaps, access to this memory is forced to use
    // ClipMemoryManager, which keeps the various pointers in sync with
    // each other

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

    // Extend mode is used to signify which end of the hovered clip
    // the cursor is over
    // This is used for deciding if a click event should extend the clip
    // Note: This variable is not used during the actual extend event!
    
    enum class ExtendMode
    {
        NONE,
        LEFT,
        RIGHT
    };

    ExtendMode extend_mode = ExtendMode::NONE;

    // Not storing the project here is a deliberate choice
    // The Editor class needs to be able to nullify the Project pointer
    // whenever the Project is locked

public:

    TimelineModule(Editor& editor);
    void reset();

    virtual void apply_bounds() override;
    virtual void apply_ui_scale() override;

    void scroll_left();
    void scroll_right();
    void zoom_in();
    void zoom_out();
    void playhead_forward(VideoTime time);
    void playhead_backward(VideoTime time);
    void playhead_to_start();
    void playhead_to_end();
    void focus_playhead();

    void select_all();
    void deselect_all();
    
    virtual void on_update() override;
    virtual void on_mouse_press(sf::Vector2i position, bool focused, InputButton button) override;
    virtual void on_mouse_move(sf::Vector2i position, bool focused, DragMouseEvent* drag_event) override;
    virtual void on_mouse_release(sf::Vector2i position, bool focused, InputButton button, DragMouseEvent* drag_event) override;
    virtual void on_mouse_drop(sf::Vector2i position, DragMouseEvent* drag_event) override;

private:

    Project* get_project();

    bool is_position_in_scroll(sf::Vector2i relative_pos);
    float x_to_time(int x);
    float time_to_x(VideoTime time);
    std::tuple<VideoTime, VideoTime> get_fitted_clip(VideoTime start_time, VideoTime length);

    void select_clip(Clip* clip);
    void deselect_clip(Clip* clip);
    void create_color_clip(VideoTime start_time, VideoTime length, sf::Color color);
    void delete_clip(Clip* clip);
    
    void update_scroll();
    void update_zoom();
    void update_scroll_color(bool hovering, bool dragging);
    void update_playhead();
};

#endif
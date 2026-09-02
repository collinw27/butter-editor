#include "editor/timeline/TimelineModule.h"

#include <algorithm>
#include <memory>

#include "utility/core.h"
#include "utility/Graphics.h"
#include "utility/Input.h"
#include "utility/FileManager.h"
#include "editor/Editor.h"
#include "editor/notifs.h"

#include "editor/core/mouse/DragScroll.h"
#include "editor/core/mouse/DragDirectScroll.h"
#include "editor/media/DragMedia.h"
#include "editor/timeline/mouse/ExtendClip.h"
#include "editor/timeline/mouse/DragPlayhead.h"

#include "project/Project.h"
#include "project/clip/Clip.h"
#include "project/clip/ColorClip.h"

#include "utility/Logger.h"

constexpr int SCROLLBAR_H = 12;
constexpr int SCROLLBAR_W = 90;
constexpr int ZOOM_MIN = -10;
constexpr int ZOOM_MAX = 8;
constexpr int START_PADDING = 10;

const sf::Color C_RULER_BG {50, 50, 50};

#define ClipMem TimelineModule::ClipMemoryManager

ClipMem::~ClipMemoryManager()
{
    clear_all();
}

void ClipMem::add_clip(TimelineClip* clip)
{
    clip_map.insert({clip->clip_id, clip});
}

void ClipMem::remove_clip(TimelineClip* clip)
{
    // Remove potential danging pointers
    // Extend mode is also reset, just to avoid any weirdness

    deselect_clip(clip);
    if (hovered_clip == clip)
        hovered_clip = nullptr;

    // The clip must be manually deleted (no smart pointer)
        
    auto it = clip_map.find(clip->clip_id);
    if (it != clip_map.end())
        clip_map.erase(it);
    delete clip;
}

void ClipMem::clear_all()
{
    // Easier to just store clips as raw pointers and delete them here
    // This prevents any interators over `clips` from having to use type std::unique_ptr<Clip>

    for (auto clip : clip_map)
    {
        delete clip.second;
    }
    clip_map.clear();

    // Don't forget to prevent danging references

    selected_clips.clear();
    hovered_clip = nullptr;
}

TimelineClip* ClipMem::get_clip(id_s clip_id)
{
    auto it = clip_map.find(clip_id);
    return (it == clip_map.end()) ? nullptr : it->second;
}

const std::unordered_map<id_s, TimelineClip*>& ClipMem::get_clips()
{
    return clip_map;
}

void ClipMem::select_clip(TimelineClip* clip)
{
    auto it = std::find(selected_clips.begin(), selected_clips.end(), clip);
    if (it == selected_clips.end())
        selected_clips.push_back(clip);
}

void ClipMem::deselect_clip(TimelineClip* clip)
{
    auto it = std::find(selected_clips.begin(), selected_clips.end(), clip);
    if (it != selected_clips.end())
        selected_clips.erase(it);
}

const std::vector<TimelineClip*>& ClipMem::get_selected_clips()
{
    return selected_clips;
}

void ClipMem::set_hovered_clip(TimelineClip* clip)
{
    hovered_clip = clip;
}

TimelineClip* ClipMem::get_hovered_clip()
{
    return hovered_clip;
}

TimelineModule::TimelineModule(Editor& editor)
    : EditorModule(editor)
{
    // Ruler appears below any clips
    // The main reason is so the start padding can be layered over it,
    // while allowing the clips to be layered over the padding

    ruler.reset(GLRectangle::create(container.get()));
    ruler->set_fill_color(C_RULER_BG);
    padding_rect.reset(GLOutlinedRectangle::create(container.get()));
    padding_rect->set_fill_color(sf::Color::Black);
    padding_rect->set_outline_color(Editor::C_BORDER);
    padding_rect->set_outline_thickness(1);

    clips_scaler.reset(GLNode::create(container.get()));
    clips_anchor.reset(GLNode::create(clips_scaler.get()));
    clips_scaler->set_position(sf::Vector2f(START_PADDING, 0));

    clip_layer.reset(GLNode::create(clips_anchor.get()));
    outline_layer.reset(GLNode::create(clips_anchor.get()));
    selection_layer.reset(GLNode::create(clips_anchor.get()));

    ghost_clip.reset(GLRectangle::create(clip_layer.get(), sf::Vector2f(), sf::Vector2f(400, 100)));
    ghost_clip->set_fill_color(sf::Color::White);
    ghost_clip->set_visible(false);

    playhead.reset(GLNode::create(clips_anchor.get()));
    tex_playhead.reset(new GLTexture(FileManager().get_res_path("tex/playhead.png")));
    playhead_sprite.reset(GLSprite::create(playhead.get(), tex_playhead.get()));
    playhead_sprite->set_position(-playhead_sprite->get_size().componentWiseMul(sf::Vector2f(0.5, 1.0)));
    playhead_line.reset(GLRectangle::create(playhead.get()));
    playhead_line->set_fill_color(sf::Color::White);
    update_playhead();

    // Scroll is in front of everything
    
    scroll_bar.reset(GLRectangle::create(container.get()));
    scroll_bar->set_fill_color(Editor::C_SCROLL_STILL);

    // Sync with project updates

    opt_into_notifs(NOTIF_TIMELINE::ID);

    reset();
}

void TimelineModule::reset()
{
    // Clear pre-existing clips

    clip_mem.clear_all();

    // Project must exist

    Project* project = get_project();
    
    for (int i = 0; i < project->get_clip_total(); ++i)
    {
        id_s clip_id = project->get_clip_at_index(i);
        TimelineClip* new_clip = new TimelineClip(clip_id, clip_layer.get());
        new_clip->set_clip_start(project->get_clip_start(clip_id));
        new_clip->set_clip_length(project->get_clip_length(clip_id));
        new_clip->set_thumbnail_color(project->get_clip_color(clip_id));
        clip_mem.add_clip(new_clip);
    }
    
    // Reset parameters

    scroll_max = std::max<VideoTime>(project->get_project_length(), 200);
    scroll_pct = 0;
    zoom_factor = 2;

    update_scroll();
    update_zoom();
}

void TimelineModule::apply_bounds()
{
    update_scroll();
    ruler->set_size(sf::Vector2f(bounds.size.x, intcast(30 * ui_scale)));
    padding_rect->set_size(sf::Vector2f(START_PADDING, bounds.size.y));
    playhead_line->set_size(sf::Vector2f(1, bounds.size.y));
    
    clips_y = ruler->get_size().y + 10;
    clip_layer->set_position(sf::Vector2f(0, clips_y));
    outline_layer->set_position(sf::Vector2f(0, clips_y));
    selection_layer->set_position(sf::Vector2f(0, clips_y));
    update_playhead();
}

void TimelineModule::apply_ui_scale()
{
    update_scroll();
    ruler->set_size(sf::Vector2f(bounds.size.x, intcast(30 * ui_scale)));
    
    clips_y = ruler->get_size().y + 10;
    clip_layer->set_position(sf::Vector2f(0, clips_y));
    outline_layer->set_position(sf::Vector2f(0, clips_y));
    selection_layer->set_position(sf::Vector2f(0, clips_y));
    update_playhead();
}

void TimelineModule::scroll_left()
{
    float pct_on_screen = scroll_span / (float) scroll_max / std::pow(2.0, zoom_factor);
    scroll_pct = clamp<float>(scroll_pct - pct_on_screen * 0.1f, 0.f, 1.f);
    update_scroll();
}

void TimelineModule::scroll_right()
{
    float pct_on_screen = scroll_span / (float) scroll_max / std::pow(2.0, zoom_factor);
    scroll_pct = clamp<float>(scroll_pct + pct_on_screen * 0.1f, 0.f, 1.f);
    update_scroll();
}

void TimelineModule::zoom_in()
{
    zoom_factor = clamp(zoom_factor + 1, ZOOM_MIN, ZOOM_MAX);
    update_zoom();
}

void TimelineModule::zoom_out()
{
    zoom_factor = clamp(zoom_factor - 1, ZOOM_MIN, ZOOM_MAX);
    update_zoom();
}

void TimelineModule::playhead_forward(VideoTime time)
{
    playhead_time = clamp(playhead_time + time, 0, scroll_max);
    update_playhead();
}

void TimelineModule::playhead_backward(VideoTime time)
{
    playhead_time = clamp(playhead_time - time, 0, scroll_max);
    update_playhead();
}

void TimelineModule::playhead_to_start()
{
    playhead_time = 0;
    update_playhead();
}

void TimelineModule::playhead_to_end()
{
    playhead_time = scroll_max;
    update_playhead();
}

// Scrolls the timeline so the playhead position is in
// the center of the screen (if possible)

void TimelineModule::focus_playhead()
{
    // An offset of 64 frames needs to be added for the playhead to be centered
    // Why this specific number?
    // Literally no clue. It was determined through experimentation.

    VideoTime offset_time = 64.0 / zoom_amount;

    VideoTime half_frames_visible = (VideoTime) (scroll_span / zoom_amount * 0.5);
    VideoTime scroll_time = clamp<VideoTime>(playhead_time - half_frames_visible - offset_time, 0, scroll_max);
    scroll_pct = scroll_time / (float) scroll_max;
    update_scroll();
}

void TimelineModule::select_all()
{
    for (std::pair<id_s, TimelineClip*> clip : clip_mem.get_clips())
    {
        select_clip(clip.second);
    }
}

void TimelineModule::deselect_all()
{
    for (std::pair<id_s, TimelineClip*> clip : clip_mem.get_clips())
    {
        deselect_clip(clip.second);
    }
}

void TimelineModule::on_update()
{
    if (has_focus)
    {
        // Select & deselect all

        if (Input().check_key_press(SF_KEY::A))
            select_all();
        if (Input().check_key_press(SF_KEY::D))
            deselect_all();

        // Playhead to start/end

        if (Input().check_key_press(SF_KEY::Home))
            playhead_to_start();
        if (Input().check_key_press(SF_KEY::End))
            playhead_to_end();

        // Focus playhead

        if (Input().check_key_press(SF_KEY::F))
            focus_playhead();
        
        // Attempt to delete selection

        if (Input().check_key(SF_KEY::Delete))
        {
            // Selected clips vector must be cloned, since it will be modified
            // in-place during this operation

            std::vector selected_clips_copy {clip_mem.get_selected_clips()};
            for (TimelineClip* clip : selected_clips_copy)
            {
                delete_clip(clip);
            }
        }
    }
}

void TimelineModule::on_mouse_press(sf::Vector2i position, bool focused, InputButton button)
{
    if (focused)
    {
        if (button == InputButton::LEFT)
        {
            // Drag scroll

            if (is_position_in_scroll(position))
            {
                editor.set_drag_event(std::unique_ptr<DragScroll>(new DragScroll(this, scroll_pct)));
                update_scroll_color(false, true);
            }

            // Drag playhead

            else if (sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(ruler->get_size())).contains(position))
            {
                editor.set_drag_event(std::unique_ptr<DragPlayhead>(new DragPlayhead(this)));
                editor.set_cursor(sf::Cursor::Type::SizeHorizontal);
                playhead_time = std::max<VideoTime>(x_to_time(position.x), 0);
                update_playhead();
            }

            else
            {
                // Select hovered clip
                // Ctrl to select multiple
                // Clicking with no hovered clip will deselect all

                if (!Input().check_ctrl())
                    deselect_all();
                
                if (clip_mem.get_hovered_clip() == nullptr)
                {
                    // Extended indicator is cleared if a nonexistent clip is clicked
                    // This can happen if a clip is deleted while the mouse is hovering it
                    
                    extend_mode = ExtendMode::NONE;
                    editor.set_cursor(sf::Cursor::Type::Arrow);
                }
                else
                {
                    if (extend_mode == ExtendMode::NONE)
                    {
                        // Select the clip without creating an extend event

                        select_clip(clip_mem.get_hovered_clip());
                    }
                    else
                    {
                        // Only the hovered clip will be extended (for now)

                        TimelineClip* extend_clip = clip_mem.get_hovered_clip();
                        deselect_all();
                        select_clip(extend_clip);

                        // The limit of the extension depends on the available gap in the timeline
                        // This can be 0

                        Project* project = get_project();
                        bool forward = (extend_mode == ExtendMode::RIGHT);
                        if (forward)
                        {
                            VideoTime start_time = project->get_clip_end(extend_clip->clip_id);
                            VideoTime gap = project->get_gap_ahead(project->get_clip_end(extend_clip->clip_id));
                            VideoTime max_trim = project->get_clip_length(extend_clip->clip_id) - 1;
                            editor.set_drag_event(std::unique_ptr<ExtendClip>(new ExtendClip(this, forward, start_time, gap, max_trim)));
                        }
                        else
                        {
                            VideoTime start_time = project->get_clip_start(extend_clip->clip_id);
                            VideoTime gap = project->get_gap_behind(project->get_clip_start(extend_clip->clip_id));
                            VideoTime max_trim = project->get_clip_length(extend_clip->clip_id) - 1;
                            editor.set_drag_event(std::unique_ptr<ExtendClip>(new ExtendClip(this, forward, start_time, gap, max_trim)));
                        }
                    }
                }
            }
        }
        
        // Drag timeline (if holding middle mouse)

        if (button == InputButton::MIDDLE)
        {
            editor.set_drag_event(std::unique_ptr<DragDirectScroll>(new DragDirectScroll(this, position.x, scroll_pct)));
        }
    }
}

void TimelineModule::on_mouse_move(sf::Vector2i position, bool focused, DragMouseEvent* drag_event)
{
    bool doing_drag_event = (drag_event != nullptr);
    if (drag_event != nullptr)
    {
        // Update scroll

        auto scroll_event = dynamic_cast<DragScroll*>(drag_event);
        if (scroll_event != nullptr)
        {
            scroll_pct = clamp<float>(scroll_event->get_scroll_offset() + scroll_event->get_total_offset().x / (float) scroll_span, 0.f, 1.f);
            update_scroll();
        }
        update_scroll_color(is_position_in_scroll(position), scroll_event != nullptr);

        // Update direct drag

        if (auto dd_event = dynamic_cast<DragDirectScroll*>(drag_event))
        {
            // Find the difference in x positions,  then convert it to scroll percentage

            int x_delta = position.x - dd_event->get_start_pos();
            float x_pct_delta = x_delta / (float) scroll_max / zoom_amount;
            scroll_pct = clamp<float>(dd_event->get_start_pct() - x_pct_delta, 0.f, 1.f);
            update_scroll();
        }

        // Update playhead drag

        if (auto playhead_event = dynamic_cast<DragPlayhead*>(drag_event))
        {
            // Translate the clicked position to the time
            
            playhead_time = std::max<VideoTime>(x_to_time(position.x), 0);
            update_playhead();
        }

        // Update clip extend

        if (auto extend_event = dynamic_cast<ExtendClip*>(drag_event))
        {
            // Cancel event if no selection

            if (clip_mem.get_selected_clips().empty())
                editor.cancel_drag_event();
            else
            {
                // Calculate the amount the mouse has dragged
                // This depends on the extension direction
                // Drag total can be negative, but should not leave the clip
                // with 0 length

                int x_diff = (extend_event->get_total_offset().x) * (extend_event->forward ? 1 : -1);
                VideoTime time_diff = (VideoTime) (x_diff / zoom_amount);
                time_diff = clamp(time_diff, -extend_event->max_trim, extend_event->max_extend);

                TimelineClip* selected_clip = clip_mem.get_selected_clips().front();
                Project* project = get_project();
                if (extend_event->forward)
                {
                    project->set_clip_end(selected_clip->clip_id, extend_event->start_time + time_diff);
                    selected_clip->render_selected(outline_layer.get(), zoom_amount);
                }
                else
                {
                    project->set_clip_start(selected_clip->clip_id, extend_event->start_time - time_diff);
                    selected_clip->render_selected(outline_layer.get(), zoom_amount);
                }

                // The scroll & project aren't updated here
                // This only occurs when the event is released
                // This prevents weird behavior from the scroll updating while
                // dragging past the end of the project
            }
        }
    }

    // Special behavior for dragging media into timeline
    // This event isn't technically targeted to this module

    if (auto drag_media_event = dynamic_cast<DragMedia*>(editor.get_drag_event()))
    {
        if (focused)
        {
            doing_drag_event = true;
            VideoTime drag_time = x_to_time(position.x);

            // Adjust the clip to be in the correct bounds
            // See `get_fitted_clip()` for more information on the procedure for this

            std::tuple<VideoTime, VideoTime> clip_bounds = get_fitted_clip(drag_time, 60);
            VideoTime clip_start = std::get<0>(clip_bounds);
            VideoTime clip_length = std::get<1>(clip_bounds);
            ghost_clip->set_visible(clip_length > 0);
            if (clip_length > 0)
            {
                drag_media_event->start_time = clip_start;
                drag_media_event->length = clip_length;
                ghost_clip->set_position(sf::Vector2f((int) clip_start, 0));
                ghost_clip->set_size(sf::Vector2f((int) clip_length, 100));
            }
            drag_media_event->valid = (clip_length > 0);
        }
    }

    // These events should only activate if not already dragging something

    if (!doing_drag_event)
    {
        // Highlight moused-over clip
        // ClipMemoryManager prevents this from becoming a dangling pointer
        // When hovering over the edge of a clip (either end), the mouse type is changed
        // and this information is stored

        float time_pos_f = x_to_time(position.x);
        int time_pos = (int) time_pos_f;
        float extend_margin_l = x_to_time(position.x - 12);
        float extend_margin_r = std::ceil(x_to_time(position.x + 12));
        if (clip_mem.get_hovered_clip() != nullptr)
        {
            clip_mem.get_hovered_clip()->set_hovering(false);
            clip_mem.set_hovered_clip(nullptr);
            extend_mode = ExtendMode::NONE;
        }

        // This could benefit from some optimization (since it currently performs 3 checks on every clip),
        // but that's a task for the far future

        if (focused && position.y >= clips_y && position.y < clips_y + 100)
        {
            for (std::pair<id_s, TimelineClip*> clip : clip_mem.get_clips())
            {
                if (clip.second->is_start_within(extend_margin_l, time_pos_f))
                {
                    clip_mem.set_hovered_clip(clip.second);
                    extend_mode = ExtendMode::LEFT;
                    clip.second->set_hovering(true);
                    break;
                }
                if (clip.second->is_end_within(time_pos_f, extend_margin_r))
                {
                    clip_mem.set_hovered_clip(clip.second);
                    extend_mode = ExtendMode::RIGHT;
                    clip.second->set_hovering(true);
                    break;
                }
                if (clip.second->is_time_within(time_pos))
                {
                    clip_mem.set_hovered_clip(clip.second);
                    extend_mode = ExtendMode::NONE;
                    clip.second->set_hovering(true);
                    break;
                }
            }
        }
        editor.set_cursor((extend_mode == ExtendMode::NONE) ? sf::Cursor::Type::Arrow : sf::Cursor::Type::SizeHorizontal);

        // Holding space will select any hovered clip

        if (Input().check_key(SF_KEY::Space))
        {
            if (clip_mem.get_hovered_clip() != nullptr)
                select_clip(clip_mem.get_hovered_clip());
        }
    }
}

void TimelineModule::on_mouse_release(sf::Vector2i position, bool focused, InputButton button, DragMouseEvent* drag_event)
{
    auto extend_event = dynamic_cast<ExtendClip*>(drag_event);
    if (extend_event != nullptr)
    {
        // When extending a clip, timeline updates are buffered until this point

        Project* project = get_project();
        VideoTime old_scroll_max = scroll_max;
        scroll_max = std::max<VideoTime>(project->get_project_length(), 200);
        if (old_scroll_max != scroll_max)
        {
            update_scroll();
            editor.on_timeline_update();
            extend_mode = ExtendMode::NONE;
            editor.set_cursor(sf::Cursor::Type::Arrow);
            TimelineClip* hovered_clip = clip_mem.get_hovered_clip();
            if (hovered_clip != nullptr)
                hovered_clip->set_hovering(false);
            clip_mem.set_hovered_clip(nullptr);
        }
    }
    
    auto playhead_event = dynamic_cast<DragPlayhead*>(drag_event);
    if (playhead_event != nullptr)
    {
        editor.set_cursor(sf::Cursor::Type::Arrow);
    }

    ghost_clip->set_visible(false);
    update_scroll_color(focused && is_position_in_scroll(position), false);
}

void TimelineModule::on_mouse_drop(sf::Vector2i position, DragMouseEvent* drag_event)
{
    if (auto drag_media_event = dynamic_cast<DragMedia*>(drag_event))
    {
        // The media drop should use the same paramters as the ghost clip

        if (drag_media_event->valid)
        {
            create_color_clip(drag_media_event->start_time, drag_media_event->length, drag_media_event->media_color);
        }
        ghost_clip->set_visible(false);
    }
}

void TimelineModule::on_notif(int notif_class, int notif_type, size_t num_args, void** arg_ptrs)
{
    if (notif_class == NOTIF_TIMELINE::ID)
    {
        switch (notif_type)
        {
        case NOTIF_TIMELINE::CLIP_BOUNDS_CHANGED:
        {
            id_s clip_id = *((id_s*) arg_ptrs[0]);
            TimelineClip* clip = clip_mem.get_clip(clip_id);
            Project* project = editor.get_project();
            clip->set_clip_start(project->get_clip_start(clip_id));
            clip->set_clip_length(project->get_clip_length(clip_id));
        }
        break;
        case NOTIF_TIMELINE::CLIP_DELETED:
        {
            // GLNodes should automatically be removed by their destructor

            id_s clip_id = *((id_s*) arg_ptrs[0]);
            TimelineClip* deleted_clip = clip_mem.get_clip(clip_id);
            clip_mem.remove_clip(deleted_clip);
        }
        break;
        }
    }
}

Project* TimelineModule::get_project()
{
    Project* project = editor.get_project();
    if (project == nullptr)
        throw ButterException("Timeline method called on null/inaccessible project");
    return project;
}

bool TimelineModule::is_position_in_scroll(sf::Vector2i relative_pos)
{
    sf::IntRect rect {sf::Vector2i(scroll_bar->get_position()), sf::Vector2i(scroll_bar->get_size())};
    return rect.contains(relative_pos);
}

// Swapping between X and TIME is pretty simple
// The one caveat is that a little padding is added before the
// beginning of the timeline when scroll is at the beginning
// This remains constant regardless of zoom

float TimelineModule::x_to_time(int pos_x)
{
    float x_scaled = (pos_x - START_PADDING) / zoom_amount;
    float x_shifted = x_scaled + scroll_amount;
    return x_shifted;
}

float TimelineModule::time_to_x(VideoTime time)
{
    float time_shifted = (float) time - scroll_amount;
    float time_scaled = time_shifted * zoom_amount + START_PADDING;
    return time_scaled;
}

// This function is used when a clip is dropped between other clips
// It returns the start time & length where the clip can be placed without
// cutting into other clips (or returns length 0 if it cannot be placed)
// At a high level, the procedure is as follows:
// 1) Attempt to place it normally
// 2) If the start is not inside a clip, but the clip cuts into another clip,
// snap the clip backwards (attempting to retain the length, but trimming if necessary)
// 3) If the start is inside the clip but there is a gap within the bounds of the clip,
// push the clip ahead to start at the gap (trimming if necessary)
// 4) If neither of these worked, we are attempting to place a clip entirely within the
// bounds of another clip (or chain of clips), so placement fails

std::tuple<VideoTime, VideoTime> TimelineModule::get_fitted_clip(VideoTime start_time, VideoTime length)
{
    Project* project = get_project();

    // (1) & (2) take place if the start position is free

    if (project->get_clip_at_time(start_time) == ID_NULL)
    {
        // 1) Attempt to place it normally

        VideoTime gap_ahead = project->get_gap_ahead(start_time);
        if (gap_ahead >= length)
            return std::tuple<VideoTime, VideoTime>(start_time, length);

        // 2) Attempt to snap the clip backwards
        // The else branch trims the clip
        
        VideoTime gap_behind = project->get_gap_behind(start_time);
        if (gap_behind + gap_ahead >= length)
            return std::tuple<VideoTime, VideoTime>(start_time - (length - gap_ahead), length);
        else
            return std::tuple<VideoTime, VideoTime>(start_time - gap_behind, gap_behind + gap_ahead);
    }

    // (3) & (4) take place if the start position is occupied

    else
    {
        VideoTime chain_ahead = project->get_chain_ahead(start_time);

        // (3) Attempt to snap the clip forwards
        // The else branch trims the clip

        if (chain_ahead < length)
        {
            VideoTime gap_further_ahead = project->get_gap_ahead(start_time + chain_ahead);
            return std::tuple<VideoTime, VideoTime>(start_time + chain_ahead, std::min(length, gap_further_ahead));
        }

        // (4) Return if filled area extends past bounds

        return std::tuple<VideoTime, VideoTime>(0, 0);
    }
}

void TimelineModule::select_clip(TimelineClip* clip)
{
    if (!clip->selected())
    {
        clip->render_selected(outline_layer.get(), zoom_amount);
        clip->get_rect()->reparent(selection_layer.get());
        clip_mem.select_clip(clip);
    }
}

void TimelineModule::deselect_clip(TimelineClip* clip)
{
    if (clip->selected())
    {
        clip->deselect();
        clip->get_rect()->reparent(clip_layer.get());
        clip_mem.deselect_clip(clip);
    }
}

void TimelineModule::create_color_clip(VideoTime start_time, VideoTime length, sf::Color color)
{
    Project* project = get_project();
    id_s clip_id = project->add_color_clip(start_time, length, color);
    TimelineClip* new_clip = new TimelineClip(clip_id, clip_layer.get());
    new_clip->set_clip_start(project->get_clip_start(clip_id));
    new_clip->set_clip_length(project->get_clip_length(clip_id));
    new_clip->set_thumbnail_color(project->get_clip_color(clip_id));
    clip_mem.add_clip(new_clip);
}

void TimelineModule::delete_clip(TimelineClip* clip)
{
    Project* project = get_project();
    project->delete_clip(clip->clip_id);
}

// `update_scroll()` and `update_zoom()` are split into two different functions
// since scrolling is generally a faster operation than zooming, and it's ideal
// for scrolling to feel as smooth as possible

void TimelineModule::update_scroll()
{
    // First, adjust the horizontal positioning of the clips

    scroll_bar->set_size(sf::Vector2f(SCROLLBAR_W, SCROLLBAR_H) * ui_scale);
    scroll_span = container->get_size().x - 10 - scroll_bar->get_size().x;

    scroll_amount = scroll_pct * scroll_max;
    clips_anchor->set_position(sf::Vector2f(-scroll_amount, 0));
    scroll_bar->set_position(sf::Vector2f(5 + intcast(scroll_pct * scroll_span), container->get_size().y - 5 - scroll_bar->get_size().y));

    padding_rect->set_position(sf::Vector2f(time_to_x(0) - START_PADDING, 0));
}

void TimelineModule::update_zoom()
{
    // Every change in zoom level doubles/halves the scale
    // Width is standardized across framerates to match 30 FPS
    // (since having the zoom depend on framerate would be strange)
    // Every 1 second is 30 px wide on 1x zoom

    Project* project = get_project();
    zoom_amount = std::pow(2.0, zoom_factor) * 30.0 / (float) project->get_framerate();
    clips_scaler->set_scale(sf::Vector2f(zoom_amount, 1.0));
    padding_rect->set_position(sf::Vector2f(time_to_x(0) - START_PADDING, 0));
    update_playhead();

    // Re-scale selection windows

    for (TimelineClip* clip : clip_mem.get_selected_clips())
    {
        clip->render_selected(outline_layer.get(), zoom_amount);
    }
}

void TimelineModule::update_scroll_color(bool hovering, bool dragging)
{
    scroll_bar->set_fill_color(dragging ? Editor::C_SCROLL_DRAG : (hovering ? Editor::C_SCROLL_HOVER : Editor::C_SCROLL_STILL));
}

void TimelineModule::update_playhead()
{
    playhead->set_position(sf::Vector2f(playhead_time, ruler->get_size().y + 2));
    playhead->set_scale(sf::Vector2f(1.0 / clips_scaler->get_scale().x, 1.0));
}
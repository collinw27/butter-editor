#include "editor/timeline/TimelineModule.h"

#include <algorithm>
#include <memory>

#include "utility/core.h"
#include "utility/Graphics.h"
#include "utility/Input.h"
#include "editor/Editor.h"
#include "editor/core/mouse/DragScroll.h"
#include "editor/core/mouse/DragDirectScroll.h"
#include "editor/media/DragMedia.h"
#include "editor/timeline/mouse/ExtendClip.h"

#include "project/Project.h"
#include "project/clip/ClipData.h"
#include "project/clip/ColorClipData.h"

#include "utility/Logger.h"

constexpr int SCROLLBAR_H = 12;
constexpr int SCROLLBAR_W = 90;
constexpr int ZOOM_MIN = -10;
constexpr int ZOOM_MAX = 8;

#define ClipMem TimelineModule::ClipMemoryManager

ClipMem::~ClipMemoryManager()
{
    clear_all();
}

void ClipMem::add_clip(Clip* clip)
{
    clips.push_back(clip);
}

void ClipMem::remove_clip(Clip* clip)
{
    // Remove potential danging pointers
    // Extend mode is also reset, just to avoid any weirdness

    deselect_clip(clip);
    if (hovered_clip == clip)
        hovered_clip = nullptr;

    // The clip must be manually deleted (no smart pointer)
        
    auto it = std::find(clips.begin(), clips.end(), clip);
    if (it != clips.end())
        clips.erase(it);
    delete clip;
}

void ClipMem::clear_all()
{
    // Easier to just store clips as raw pointers and delete them here
    // This prevents any interators over `clips` from having to use type std::unique_ptr<Clip>

    for (auto clip : clips)
    {
        delete clip;
    }
    clips.clear();

    // Don't forget to prevent danging references

    selected_clips.clear();
    hovered_clip = nullptr;
}

const std::vector<Clip*>& ClipMem::get_clips()
{
    return clips;
}

void ClipMem::select_clip(Clip* clip)
{
    auto it = std::find(selected_clips.begin(), selected_clips.end(), clip);
    if (it == selected_clips.end())
        selected_clips.push_back(clip);
}

void ClipMem::deselect_clip(Clip* clip)
{
    auto it = std::find(selected_clips.begin(), selected_clips.end(), clip);
    if (it != selected_clips.end())
        selected_clips.erase(it);
}

const std::vector<Clip*>& ClipMem::get_selected_clips()
{
    return selected_clips;
}

void ClipMem::set_hovered_clip(Clip* clip)
{
    hovered_clip = clip;
}

Clip* ClipMem::get_hovered_clip()
{
    return hovered_clip;
}

TimelineModule::TimelineModule(Editor& editor)
    : EditorModule(editor)
{
    clips_scaler.reset(GLNode::create(container.get()));
    clips_anchor.reset(GLNode::create(clips_scaler.get()));
    clip_layer.reset(GLNode::create(clips_anchor.get()));
    outline_layer.reset(GLNode::create(clips_anchor.get()));
    selection_layer.reset(GLNode::create(clips_anchor.get()));
    
    scroll_bar.reset(GLRectangle::create(container.get()));
    scroll_bar->set_fill_color(Editor::C_SCROLL_STILL);

    ghost_clip.reset(GLRectangle::create(clip_layer.get(), sf::Vector2f(), sf::Vector2f(400, 100)));
    ghost_clip->set_fill_color(sf::Color::White);
    ghost_clip->set_visible(false);

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
        ClipData* clip_data = project->get_clip_at_index(i);
        Clip* new_clip = new Clip(clip_data, ((ColorClipData*) clip_data)->get_color(), clip_layer.get());
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
}

void TimelineModule::apply_ui_scale()
{
    update_scroll();
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

void TimelineModule::select_all()
{
    for (Clip* clip : clip_mem.get_clips())
    {
        select_clip(clip);
    }
}

void TimelineModule::deselect_all()
{
    for (Clip* clip : clip_mem.get_clips())
    {
        deselect_clip(clip);
    }
}

void TimelineModule::on_update()
{
    if (has_focus)
    {
        // Select & deselect all

        if (Input().check_key_press(SF_KEY::A, KeyMod::CTRL))
            select_all();
        
        if (Input().check_key_press(SF_KEY::Escape))
            deselect_all();
        
        // Attempt to delete selection

        if (Input().check_key(SF_KEY::Delete))
        {
            // Selected clips vector must be cloned, since it will be modified
            // in-place during this operation

            std::vector selected_clips_copy {clip_mem.get_selected_clips()};
            for (Clip* clip : selected_clips_copy)
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

                        Clip* extend_clip = clip_mem.get_hovered_clip();
                        deselect_all();
                        select_clip(extend_clip);

                        // The limit of the extension depends on the available gap in the timeline
                        // This can be 0

                        Project* project = get_project();
                        bool forward = (extend_mode == ExtendMode::RIGHT);
                        if (forward)
                        {
                            VideoTime start_time = extend_clip->get_clip_data()->get_end_time();
                            VideoTime gap = project->get_gap_ahead(extend_clip->get_clip_data()->get_end_time());
                            VideoTime max_trim = extend_clip->get_clip_data()->get_length() - 1;
                            editor.set_drag_event(std::unique_ptr<ExtendClip>(new ExtendClip(this, forward, start_time, gap, max_trim)));
                        }
                        else
                        {
                            VideoTime start_time = extend_clip->get_clip_data()->get_start_time();
                            VideoTime gap = project->get_gap_behind(extend_clip->get_clip_data()->get_start_time());
                            VideoTime max_trim = extend_clip->get_clip_data()->get_length() - 1;
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

                Clip* selected_clip = clip_mem.get_selected_clips().front();
                Project* project = get_project();
                if (extend_event->forward)
                {
                    selected_clip->set_clip_end(project, extend_event->start_time + time_diff);
                    selected_clip->render_selected(outline_layer.get(), zoom_amount);
                }
                else
                {
                    selected_clip->set_clip_start(project, extend_event->start_time - time_diff);
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
                ghost_clip->set_position(sf::Vector2f((int) clip_start, 40));
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

        if (focused && position.y >= 40 && position.y < 140)
        {
            for (Clip* clip : clip_mem.get_clips())
            {
                if (clip->is_start_within(extend_margin_l, time_pos_f))
                {
                    clip_mem.set_hovered_clip(clip);
                    extend_mode = ExtendMode::LEFT;
                    clip->set_hovering(true);
                    break;
                }
                if (clip->is_end_within(time_pos_f, extend_margin_r))
                {
                    clip_mem.set_hovered_clip(clip);
                    extend_mode = ExtendMode::RIGHT;
                    clip->set_hovering(true);
                    break;
                }
                if (clip->is_time_within(time_pos))
                {
                    clip_mem.set_hovered_clip(clip);
                    extend_mode = ExtendMode::NONE;
                    clip->set_hovering(true);
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
            Clip* hovered_clip = clip_mem.get_hovered_clip();
            if (hovered_clip != nullptr)
                hovered_clip->set_hovering(false);
            clip_mem.set_hovered_clip(nullptr);
        }
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

float TimelineModule::x_to_time(int pos_x)
{
    // Instead of performing arithmetic on every clip bounds,
    // find the time the mouse X represents within the timeline

    float x_scaled = pos_x / zoom_amount;
    float x_shifted = x_scaled + scroll_amount;
    return x_shifted;
}

float TimelineModule::time_to_x(VideoTime time)
{
    float time_shifted = (float) time - scroll_amount;
    float time_scaled = time_shifted * zoom_amount;
    return time_shifted;
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

    if (project->get_clip_at_time(start_time) == nullptr)
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

void TimelineModule::select_clip(Clip* clip)
{
    if (!clip->selected())
    {
        clip->render_selected(outline_layer.get(), zoom_amount);
        clip->get_rect()->reparent(selection_layer.get());
        clip_mem.select_clip(clip);
    }
}

void TimelineModule::deselect_clip(Clip* clip)
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
    ClipData* clip_data = project->add_color_clip(start_time, length, color);
    clip_mem.add_clip(new Clip(clip_data, color, clip_layer.get()));
}

void TimelineModule::delete_clip(Clip* clip)
{
    Project* project = get_project();
    clip->delete_clip(project);
    
    // GLNodes should automatically be removed by their destructor

    clip_mem.remove_clip(clip);
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

    // Re-scale selection windows

    for (Clip* clip : clip_mem.get_selected_clips())
    {
        clip->render_selected(outline_layer.get(), zoom_amount);
    }
}

void TimelineModule::update_scroll_color(bool hovering, bool dragging)
{
    scroll_bar->set_fill_color(dragging ? Editor::C_SCROLL_DRAG : (hovering ? Editor::C_SCROLL_HOVER : Editor::C_SCROLL_STILL));
}
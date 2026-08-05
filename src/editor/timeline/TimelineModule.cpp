#include "editor/timeline/TimelineModule.h"

#include <algorithm>
#include <memory>

#include "utility/core.h"
#include "utility/Graphics.h"
#include "utility/Input.h"
#include "editor/Editor.h"
#include "editor/core/DragScroll.h"
#include "editor/core/DragDirectScroll.h"
#include "editor/media/DragMedia.h"

#include "project/Project.h"
#include "project/clip/ClipData.h"
#include "project/clip/ColorClipData.h"
#include "editor/timeline/clip/Clip.h"

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
        delete clip;
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
    scroll_bar.reset(GLRectangle::create(container.get()));
    scroll_bar->set_fill_color(Editor::C_SCROLL_STILL);
    clip_layer.reset(GLNode::create(clips_anchor.get()));
    outline_layer.reset(GLNode::create(clips_anchor.get()));
    selection_layer.reset(GLNode::create(clips_anchor.get()));

    scroll_pct = 0;
    scroll_max = 200;
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

void TimelineModule::refresh_clips()
{
    // Clear pre-existing clips

    clip_mem.clear_all();

    // Project must exist

    Project* project = editor.get_project();
    if (project == nullptr)
        throw ButterException("Invalid call to update_zoom()");
    
    for (int i = 0; i < project->get_clip_total(); ++i)
    {
        ClipData* clip_data = project->get_clip_at_index(i);
        Clip* new_clip = new Clip(clip_data, ((ColorClipData*) clip_data)->get_color(), clip_layer.get());
        clip_mem.add_clip(new_clip);
    }
    scroll_max = std::max<int>(project->get_project_length(), 200);
    update_scroll();
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
                if (clip_mem.get_hovered_clip() != nullptr)
                    select_clip(clip_mem.get_hovered_clip());
            }
        }
        
        // Drag timeline (if holding space)

        if (button == InputButton::MIDDLE)
        {
            editor.set_drag_event(std::unique_ptr<DragDirectScroll>(new DragDirectScroll(this, position.x, scroll_pct)));
        }
    }
}

void TimelineModule::on_mouse_move(sf::Vector2i position, bool focused, DragMouse* drag_event)
{
    // Update scroll

    DragScroll* scroll_event = (drag_event) ? dynamic_cast<DragScroll*>(drag_event) : nullptr;
    if (scroll_event != nullptr)
    {
        scroll_pct = clamp<float>(scroll_event->get_scroll_offset() + scroll_event->get_total_offset().x / (float) scroll_span, 0.f, 1.f);
        update_scroll();
    }
    update_scroll_color(is_position_in_scroll(position), scroll_event != nullptr);

    // Update space drag

    DragDirectScroll* space_event = (drag_event) ? dynamic_cast<DragDirectScroll*>(drag_event) : nullptr;
    if (space_event != nullptr)
    {
        // Find the difference in x positions,  then convert it to scroll percentage

        int x_delta = position.x - space_event->get_start_pos();
        float x_pct_delta = x_delta / (float) scroll_max / zoom_amount;
        scroll_pct = clamp<float>(space_event->get_start_pct() - x_pct_delta, 0.f, 1.f);
        update_scroll();
    }

    // Highlight moused-over clip
    // ClipMemoryManager prevents this from becoming a dangling pointer

    int time_pos = (int) x_to_time(position.x);
    if (clip_mem.get_hovered_clip() != nullptr)
    {
        clip_mem.get_hovered_clip()->set_hovering(false);
        clip_mem.set_hovered_clip(nullptr);
    }
    if (focused && position.y >= 40 && position.y < 140)
    {
        for (Clip* clip : clip_mem.get_clips())
        {
            if (clip->is_time_within(time_pos))
            {
                clip_mem.set_hovered_clip(clip);
                clip->set_hovering(true);
                break;
            }
        }
    }

    // Holding middle mouse will select any hovered clip

    if (Input().check_key(SF_KEY::Space))
    {
        if (clip_mem.get_hovered_clip() != nullptr)
            select_clip(clip_mem.get_hovered_clip());
    }
}

void TimelineModule::on_mouse_release(sf::Vector2i position, bool focused, InputButton button, DragMouse* drag_event)
{
    update_scroll_color(focused && is_position_in_scroll(position), false);
}

void TimelineModule::on_mouse_drop(sf::Vector2i position, DragMouse* drag_event)
{
    if (auto drag_media = dynamic_cast<DragMedia*>(drag_event))
    {
        // Logger().log("Dropped media");
    }
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

void TimelineModule::delete_clip(Clip* clip)
{
    Project* project = editor.get_project();
    if (project == nullptr)
        throw ButterException("Invalid call");
    clip->delete_clip(project);
    
    // GLNodes should automatically be removed by their destructor

    clip_mem.remove_clip(clip);
}

void TimelineModule::update_scroll()
{
    scroll_bar->set_size(sf::Vector2f(SCROLLBAR_W, SCROLLBAR_H) * ui_scale);
    scroll_span = container->get_size().x - 10 - scroll_bar->get_size().x;

    scroll_amount = scroll_pct * scroll_max;
    clips_anchor->set_position(sf::Vector2f(-scroll_amount, 0));
    scroll_bar->set_position(sf::Vector2f(5 + intcast(scroll_pct * scroll_span), container->get_size().y - 5 - scroll_bar->get_size().y));

    Project* project = editor.get_project();
    if (project == nullptr)
        throw ButterException("Invalid call to update_zoom()");
    TimelineUnit start_time = (TimelineUnit) (scroll_pct * scroll_max);
}

void TimelineModule::update_zoom()
{
    // Every change in zoom level doubles/halves the scale
    // Width is standardized across framerates to match 30 FPS
    // (since having the zoom depend on framerate would be strange)
    // Every 1 second is 30 px wide on 1x zoom

    Project* project = editor.get_project();
    if (project == nullptr)
        throw ButterException("Invalid call to update_zoom()");
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
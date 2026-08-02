#include "editor/timeline/TimelineModule.h"

#include <algorithm>
#include <memory>

#include "utility/core.h"
#include "utility/Graphics.h"
#include "editor/Editor.h"
#include "editor/core/DragScroll.h"
#include "editor/media/DragMedia.h"

#include "project/Project.h"
#include "project/clip/ClipData.h"
#include "project/clip/ColorClipData.h"
#include "editor/timeline/clip/Clip.h"

constexpr int SCROLLBAR_H = 12;
constexpr int SCROLLBAR_W = 90;
constexpr int ZOOM_MIN = -10;
constexpr int ZOOM_MAX = 8;

TimelineModule::TimelineModule(Editor& editor)
    : EditorModule(editor)
{
    clips_scaler = GLNode::create(container);
    clips_anchor = GLNode::create(clips_scaler);
    scroll_bar = GLRectangle::create(container);
    scroll_bar->set_fill_color(Editor::C_SCROLL_STILL);

    // start_text = GLText::create(container, Graphics().mono_font(), 14u, "00:00:00+00");
    // start_text->set_position(sf::Vector2f(8, 8));

    scroll_pct = 0;
    scroll_max = 200;
    zoom_factor = 2;

    update_scroll();
    update_zoom();
}

TimelineModule::~TimelineModule()
{
    delete clips_anchor;
    delete scroll_bar;
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

    clips.clear();

    Project* project = editor.get_project();
    if (project == nullptr)
        return;
    
    for (int i = 0; i < project->get_clip_total(); ++i)
    {
        ClipData* clip_data = project->get_clip_at_index(i);
        Clip* new_clip = new Clip(clip_data, ((ColorClipData*) clip_data)->get_color(), clips_anchor);
        clips.push_back(std::unique_ptr<Clip>(new_clip));
    }
    scroll_max = std::max<int>(project->get_project_length(), 200);
    update_scroll();
    update_zoom();
}

void TimelineModule::on_mouse_press(sf::Vector2i position, bool focused)
{
    if (focused && is_position_in_scroll(position))
    {
        editor.set_drag_event(std::unique_ptr<DragScroll>(new DragScroll(this, scroll_pct)));
        update_scroll_color(false, true);
    }
}

void TimelineModule::on_mouse_move(sf::Vector2i position, bool focused, DragMouse* drag_event)
{
    DragScroll* scroll_event = (drag_event) ? dynamic_cast<DragScroll*>(drag_event) : nullptr;
    if (scroll_event != nullptr)
    {
        scroll_pct = clamp<float>(scroll_event->get_scroll_offset() + scroll_event->get_total_offset().x / (float) scroll_span, 0.f, 1.f);
        update_scroll();
    }
    update_scroll_color(is_position_in_scroll(position), scroll_event != nullptr);
}

void TimelineModule::on_mouse_release(sf::Vector2i position, bool focused, DragMouse* drag_event)
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

void TimelineModule::update_scroll()
{
    scroll_bar->set_size(sf::Vector2f(SCROLLBAR_W, SCROLLBAR_H) * ui_scale);
    scroll_span = container->get_size().x - 10 - scroll_bar->get_size().x;

    clips_anchor->set_position(sf::Vector2f(-scroll_pct * scroll_max, 0));
    scroll_bar->set_position(sf::Vector2f(5 + intcast(scroll_pct * scroll_span), container->get_size().y - 5 - scroll_bar->get_size().y));

    Project* project = editor.get_project();
    if (!project)
        throw ButterException("Invalid call to update_zoom()");
    TimelineUnit start_time = (TimelineUnit) (scroll_pct * scroll_max);
    // start_text->set_string(project->to_string(start_time));
}

void TimelineModule::update_zoom()
{
    // Every change in zoom level doubles/halves the scale
    // Width is standardized across framerates to match 30 FPS
    // (since having the zoom depend on framerate would be strange)
    // Every 1 second is 30 px wide on 1x zoom

    Project* project = editor.get_project();
    if (!project)
        throw ButterException("Invalid call to update_zoom()");
    clips_scaler->set_scale(sf::Vector2f(std::pow(2.0, zoom_factor) * 30.0 / (float) project->get_framerate(), 1.0));
}

void TimelineModule::update_scroll_color(bool hovering, bool dragging)
{
    scroll_bar->set_fill_color(dragging ? Editor::C_SCROLL_DRAG : (hovering ? Editor::C_SCROLL_HOVER : Editor::C_SCROLL_STILL));
}
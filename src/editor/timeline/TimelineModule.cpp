#include "editor/timeline/TimelineModule.h"

#include <algorithm>
#include <memory>

#include "utility/core.h"
#include "editor/Editor.h"
#include "editor/core/DragScroll.h"
#include "editor/media/DragMedia.h"
#include "project/Project.h"
#include "project/timeline/ColorClip.h"

#include "utility/Logger.h"

constexpr int SCROLLBAR_H = 12;
constexpr int SCROLLBAR_W = 90;

TimelineModule::TimelineModule(Editor& editor)
    : EditorModule(editor)
{
    clips_anchor = GLNode::create(container);
    scroll_bar = GLRectangle::create(container);
    scroll_bar->set_fill_color(Editor::C_SCROLL_STILL);

    scroll_pct = 0;
    scroll_max = 200;
}

TimelineModule::~TimelineModule()
{
    delete clips_anchor;
    for (GLRectangle* clip : clips)
        delete clip;
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

void TimelineModule::refresh_clips()
{
    // Clear pre-existing clips

    for (GLRectangle* clip : clips)
        delete clip;
    clips.clear();

    Project* project = editor.get_project();
    if (project == nullptr)
        return;
    
    for (int i = 0; i < project->get_clip_total(); ++i)
    {
        TimelineClip* clip_data = project->get_clip_at_index(i);
        GLRectangle* new_clip = GLRectangle::create(
            clips_anchor,
            sf::Vector2f(clip_data->get_start_time() * 10, 0),
            sf::Vector2f(clip_data->get_length() * 10, 999)
        );
        new_clip->set_fill_color(((ColorClip*) clip_data)->get_color());
        clips.push_back(new_clip);
    }
    scroll_max = std::max<int>(project->get_project_length(), 200);
    update_scroll();
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

    clips_anchor->set_position(sf::Vector2f(-scroll_pct * scroll_max * 10, 0));
    scroll_bar->set_position(sf::Vector2f(5 + intcast(scroll_pct * scroll_span), container->get_size().y - 5 - scroll_bar->get_size().y));
}

void TimelineModule::update_scroll_color(bool hovering, bool dragging)
{
    scroll_bar->set_fill_color(dragging ? Editor::C_SCROLL_DRAG : (hovering ? Editor::C_SCROLL_HOVER : Editor::C_SCROLL_STILL));
}
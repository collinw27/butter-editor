#include "editor/timeline/TimelineModule.h"

#include <algorithm>

#include "editor/Editor.h"
#include "editor/config/DragMedia.h"

TimelineModule::TimelineModule(Editor& editor) :
    EditorModule{editor, sf::IntRect({0, 360}, {1280, 720-360})}
{
    timeline_rect = sf::RectangleShape(sf::Vector2f(bounds.size) - sf::Vector2f({10, 10}));
    timeline_rect.setPosition(sf::Vector2f(bounds.position) + sf::Vector2f({5, 5}));
    timeline_rect.setFillColor(sf::Color::Black);
}

void TimelineModule::update()
{
    for (auto clip : deleted_clips)
    {
        media_clips.erase(std::find(media_clips.begin(), media_clips.end(), clip));
        if (selected_clip == clip)
        {
            selected_clip = nullptr;
        }
        delete clip;
    }
    deleted_clips.clear();
}

void TimelineModule::draw(sf::RenderWindow &window)
{
    window.draw(visible_rect);
    window.draw(timeline_rect);
    
    // Draw selected clip last to make sure border
    // is drawn over adjacent clips

    for (auto clip : media_clips)
    {
        if (clip != selected_clip)
            clip->draw(window);
    }
    if (selected_clip != nullptr)
    {
        selected_clip->draw(window);
    }
}

void TimelineModule::on_mouse_pressed(sf::Vector2i position, bool focused)
{
    if (focused)
    {
        // Select media clip if mouse is over it
        // Otherwise, deselect any currently-selected clip

        if (selected_clip != nullptr)
        {
            selected_clip->deselect();
            selected_clip = nullptr;
        }
        for (auto clip : media_clips)
        {
            if (clip->was_clicked(position))
            {
                clip->select();
                selected_clip = clip;
            }
        }
    }
}

void TimelineModule::on_mouse_released(sf::Vector2i position, bool focused, DragMouse *drag_mouse_event)
{
    if (focused)
    {
        // Create new media clip if mouse was dragging media

        DragMedia *drag_media_event = dynamic_cast<DragMedia*>(drag_mouse_event);
        if (drag_media_event != nullptr)
        {
            float start_time = (position.x - bounds.position.x) / float(bounds.size.x);
            float end_time = start_time + 0.1;
            if (start_time > 0.0 && start_time < 0.9)
            {
                auto clip = new TimelineClip(this, drag_media_event->get_color(), start_time, 0.1);
                media_clips.push_back(clip);
            }

            // Store all clips that are cut into
            // 3 choices: trim start, trim end, fully delete

            
            for (auto clip : media_clips)
            {
                if (start_time > clip->get_start() && start_time < clip->get_end())
                {
                    clip->trim_end(start_time);
                }
                if (end_time > clip->get_start() && end_time < clip->get_end())
                {
                    clip->trim_start(end_time);
                }
                if (start_time < clip->get_start() && end_time > clip->get_end())
                {
                    remove_clip(clip);
                }
            }
        }
    }
}

sf::FloatRect TimelineModule::get_clip_bounds(float start_time, float length)
{
    return sf::FloatRect(sf::Vector2f({bounds.position.x + bounds.size.x * start_time,
        bounds.position.y + 10.f}), 
        sf::Vector2f({float(bounds.size.x) * length, 80.0f})
    );
}

void TimelineModule::remove_clip(TimelineClip *clip)
{
    deleted_clips.push_back(clip);
}

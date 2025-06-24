#include "editor/config/TimelineModule.h"

#include "editor/Editor.h"
#include "editor/config/DragMedia.h"

TimelineModule::TimelineModule(Editor& editor) :
    EditorModule{editor, sf::IntRect({0, 360}, {1280, 720-360})}
{
    timelineRect = sf::RectangleShape(sf::Vector2f(bounds.size) - sf::Vector2f({10, 10}));
    timelineRect.setPosition(sf::Vector2f(bounds.position) + sf::Vector2f({5, 5}));
    timelineRect.setFillColor(sf::Color::Black);
}

void TimelineModule::draw(sf::RenderWindow &window)
{
    window.draw(visible_rect);
    window.draw(timelineRect);
}

void TimelineModule::on_mouse_released(sf::Vector2i position, bool focused, DragMouse *drag_mouse_event)
{
    if (focused)
    {
        DragMedia *drag_media_event = dynamic_cast<DragMedia*>(drag_mouse_event);
        if (drag_media_event != nullptr)
        {
            timelineRect.setFillColor(drag_media_event->get_color());
        }
    }
}
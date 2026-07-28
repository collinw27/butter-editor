#include "editor/core/TimelineModule.h"

#include "editor/Editor.h"
#include "project/Project.h"
#include "project/timeline/ColorClip.h"

TimelineModule::TimelineModule(Editor& editor)
    : EditorModule(editor)
{
    // test_rect = GLRectangle::create(container, sf::Vector2f(0, 0), sf::Vector2f(200, 200));
}

TimelineModule::~TimelineModule()
{
    // delete test_rect;
}

void TimelineModule::apply_bounds()
{

}

void TimelineModule::apply_ui_scale()
{

}

void TimelineModule::refresh_clips()
{
    // Clear pre-existing clips

    for (GLRectangle* clip : clips)
        delete clip;
    clips.clear();

    Project* project = editor.get_project();
    for (int i = 0; i < project->get_clip_total(); ++i)
    {
        TimelineClip* clip_data = project->get_clip_at_index(i);
        GLRectangle* new_clip = GLRectangle::create(
            container,
            sf::Vector2f(clip_data->get_start_time() * 10, 0),
            sf::Vector2f(clip_data->get_length() * 10, 999)
        );
        new_clip->set_fill_color(((ColorClip*) clip_data)->get_color());
        clips.push_back(new_clip);
    }
}

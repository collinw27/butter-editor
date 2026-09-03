#include "editor/project/ProjectModule.h"

#include "utility/FileManager.h"
#include "utility/Graphics.h"
#include "editor/Editor.h"
#include "editor/notifs.h"
#include "project/Project.h"

ProjectModule::ProjectModule(Editor& editor)
    : EditorModule(editor)
{
    text.reset(GLText::create(container.get(), Graphics().mono_font()));
    text->set_position(sf::Vector2f(8, 8));

    opt_into_notifs(NOTIF_PROJECT_INFO::ID);
}

void ProjectModule::apply_ui_scale()
{
    text->set_char_size((unsigned int)(16.f * ui_scale));
}

void ProjectModule::on_notif(int notif_class, int notif_type, size_t num_args, void** arg_ptrs)
{
    if (notif_class == NOTIF_PROJECT_INFO::ID)
    {
        if (notif_type == NOTIF_PROJECT_INFO::LENGTH_CHANGED)
        {
            refresh_info();
        }
    }
}

void ProjectModule::refresh_info()
{
    Project* project = editor.get_project();
    std::string info = "";
    info += "Name: " + project->get_name();
    info += "\nFPS: " + std::to_string(project->get_framerate());
    info += "\nResolution: " + std::to_string(project->get_resolution().x) + "x";
    info += std::to_string(project->get_resolution().y);
    info += "\nLength: " + project->get_project_length_approx();
    text->set_string(info);
}
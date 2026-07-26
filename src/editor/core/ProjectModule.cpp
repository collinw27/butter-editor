#include "editor/core/ProjectModule.h"

#include "utility/FileManager.h"
#include "utility/Graphics.h"
#include "editor/Editor.h"
#include "project/Project.h"

ProjectModule::ProjectModule(Editor& editor)
    : EditorModule(editor)
{
    text = GLText::create(container, Graphics().mono_font(), 40u, "");
    text->set_position(sf::Vector2f(8, 8));
}

void ProjectModule::apply_ui_scale()
{
    text->set_char_size((unsigned int)(16.f * ui_scale));
}

void ProjectModule::refresh_info()
{
    Project& project = editor.get_project();
    std::string info = "";
    info += "Name: " + project.get_name();
    info += "\nFPS: " + std::to_string(project.get_framerate());
    info += "\nResolution: " + std::to_string(project.get_resolution().x) + "x";
    info += std::to_string(project.get_resolution().y);
    info += "\nLength: " + editor.get_project().get_project_length_approx();
    text->set_string(info);
}
#include "editor/debug/DebugModule.h"

#include "utility/FileManager.h"
#include "utility/Graphics.h"
#include "editor/Editor.h"

DebugModule::DebugModule(Editor& editor)
    : EditorModule(editor)
{
    text.reset(GLText::create(container.get(), Graphics().mono_font(), 40u, ""));
    text->set_position(sf::Vector2f(8, 8));
}

void DebugModule::apply_ui_scale()
{
    text->set_char_size((unsigned int)(16.f * ui_scale));
}

void DebugModule::refresh_info(std::string new_info)
{
    text->set_string(new_info);
}
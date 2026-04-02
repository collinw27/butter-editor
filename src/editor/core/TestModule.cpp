#include "editor/core/TestModule.h"

#include "utility/FileManager.h"
#include "utility/Graphics.h"
#include "editor/Editor.h"

TestModule::TestModule(Editor& editor, std::string sample_text)
    : EditorModule(editor)
{
    // text = new sf::Text(FileManager().get_font(), sample_text, 30u);
    text = GLText::create(visible_rect, Graphics().main_font(), 40u, sample_text);
    text->set_position(sf::Vector2f(30, 30));
}

TestModule::~TestModule()
{
    delete text;
}

void TestModule::set_bounds(const sf::IntRect& bounds)
{
    EditorModule::set_bounds(bounds);
    // text->set_position(sf::Vector2f(this->bounds.position) + sf::Vector2f(30, 30));
}

void TestModule::set_ui_scale(float new_scale)
{
    EditorModule::set_ui_scale(new_scale);
    text->set_char_size((unsigned)(30.f * ui_scale));
}
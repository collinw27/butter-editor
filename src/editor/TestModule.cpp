#include "editor/TestModule.h"

#include "utility/FileManager.h"

TestModule::TestModule(Editor& editor, std::string sample_text)
    : EditorModule(editor)
{
    text = new sf::Text(FileManager().get_font(), sample_text, 30u);
}

TestModule::~TestModule()
{
    delete text;
    text = nullptr;
}

void TestModule::draw(sf::RenderWindow& window)
{
    window.draw(*text);
}

void TestModule::set_bounds(const sf::IntRect& bounds)
{
    EditorModule::set_bounds(bounds);
    text->setPosition(sf::Vector2f(this->bounds.position) + sf::Vector2f(30, 30));
}

void TestModule::set_ui_scale(float new_scale)
{
    EditorModule::set_ui_scale(new_scale);
    text->setCharacterSize((unsigned)(30.f * ui_scale));
}
#include "editor/core/FlexTab.h"

#include "editor/Editor.h"
#include "utility/FileManager.h"

FlexTab::FlexTab(EditorModule* module, std::string name)
{
    this->selected = selected;
    this->module = module;
    rect = sf::RectangleShape();
    rect.setFillColor(Editor::C_FG_DESELECTED);
    text = new sf::Text(FileManager().get_font(), name);
}

FlexTab::~FlexTab()
{
    // For now, this class is responsible for its corresponding module

    delete module;
}

void FlexTab::draw(sf::RenderWindow& window)
{
    window.draw(rect);
    window.draw(*text);
}

void FlexTab::set_bounds(const sf::IntRect& new_bounds)
{
    bounds = new_bounds;
    rect.setPosition(sf::Vector2f(bounds.position));
    rect.setSize(sf::Vector2f(bounds.size));

    // A Vector2i is used within a Vector2f constructor to round the position
    // to a whole-pixel, preventing blurriness
    
    text->setPosition(sf::Vector2f(bounds.position + sf::Vector2i(4 * ui_scale, 2 * ui_scale)));
}

void FlexTab::set_ui_scale(float new_scale)
{
    ui_scale = new_scale;
    text->setPosition(sf::Vector2f(bounds.position + sf::Vector2i(4 * ui_scale, 2 * ui_scale)));
    text->setCharacterSize((unsigned)(19.f * ui_scale));
}

void FlexTab::set_hovering(bool hovering)
{
    if (this->hovering != hovering)
    {
        this->hovering = hovering;
        rect.setFillColor(hovering ? Editor::C_HOVER : (selected ? Editor::C_FG : Editor::C_FG_DESELECTED));
    }
}

void FlexTab::set_selected(bool selected)
{
    if (this->selected != selected)
    {
        this->selected = selected;
        rect.setFillColor(hovering ? Editor::C_HOVER : (selected ? Editor::C_FG : Editor::C_FG_DESELECTED));
    }
}

sf::IntRect FlexTab::get_bounds()
{
    return bounds;
}

EditorModule& FlexTab::get_module()
{
    return *module;
}
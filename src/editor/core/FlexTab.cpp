#include "editor/core/FlexTab.h"

#include "editor/Editor.h"
#include "utility/FileManager.h"
#include "utility/Graphics.h"

FlexTab::FlexTab(Editor& editor, EditorModule* module, std::string name)
{
    this->selected = selected;
    this->module = module;
    this->module->set_visible(false);

    container = GLNode::create(nullptr);
    rect = GLRectangle::create(container);
    rect->set_fill_color(Editor::C_FG_DESELECTED);
    text = GLText::create(container, Graphics().main_font(), 25u, name);
}

FlexTab::~FlexTab()
{
    // For now, this class is responsible for its corresponding module

    delete module;
}

GLNode* FlexTab::get_node()
{
    return container;
}

void FlexTab::set_bounds(const sf::IntRect& new_bounds)
{
    bounds = new_bounds;
    rect->set_position(sf::Vector2f(bounds.position));
    rect->set_size(sf::Vector2f(bounds.size));

    // A Vector2i is used within a Vector2f constructor to round the position
    // to a whole-pixel, preventing blurriness
    
    text->set_position(sf::Vector2f(bounds.position + sf::Vector2i(4 * ui_scale, 2 * ui_scale)));
}

void FlexTab::set_ui_scale(float new_scale)
{
    ui_scale = new_scale;
    text->set_position(sf::Vector2f(bounds.position + sf::Vector2i(4 * ui_scale, 2 * ui_scale)));
    text->set_char_size((unsigned int)(19.f * ui_scale));
}

void FlexTab::set_hovering(bool hovering)
{
    if (this->hovering != hovering)
    {
        this->hovering = hovering;
        rect->set_fill_color(hovering ? Editor::C_HOVER : (selected ? Editor::C_FG : Editor::C_FG_DESELECTED));
    }
}

void FlexTab::set_selected(bool selected)
{
    if (this->selected != selected)
    {
        this->selected = selected;
        rect->set_fill_color(hovering ? Editor::C_HOVER : (selected ? Editor::C_FG : Editor::C_FG_DESELECTED));
        this->module->set_visible(selected);
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
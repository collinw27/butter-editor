#ifndef FLEX_TAB_H
#define FLEX_TAB_H

#include "graphics/nodes.h"
#include "editor/EditorModule.h"

class FlexTab
{
    EditorModule* module;
    sf::IntRect bounds;
    float ui_scale = 1.f;
    GLNode* container;
    GLRectangle* rect;
    GLText* text;

    bool hovering = false;
    bool selected = false;

public:

    FlexTab(Editor& editor, EditorModule* module, std::string name);
    ~FlexTab();
    GLNode* get_node();

    void set_bounds(const sf::IntRect& new_bounds);
    void set_ui_scale(float new_scale);
    void set_hovering(bool hovering);
    void set_selected(bool selected);

    sf::IntRect get_bounds();
    EditorModule& get_module();
};

#endif
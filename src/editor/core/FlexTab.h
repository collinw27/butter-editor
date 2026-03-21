#ifndef FLEX_TAB_H
#define FLEX_TAB_H

#include <SFML/Graphics.hpp>
#include "editor/EditorModule.h"

class FlexTab
{
    EditorModule* module;
    sf::IntRect bounds;
    float ui_scale = 1.f;
    sf::RectangleShape rect;
    sf::Text* text;

    bool hovering = false;
    bool selected = false;

public:

    FlexTab(EditorModule* module, std::string name);
    ~FlexTab();
    void draw(sf::RenderWindow& window);

    void set_bounds(const sf::IntRect& new_bounds);
    void set_ui_scale(float new_scale);
    void set_hovering(bool hovering);
    void set_selected(bool selected);

    sf::IntRect get_bounds();
    EditorModule& get_module();
};

#endif
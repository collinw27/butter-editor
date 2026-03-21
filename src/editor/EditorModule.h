#ifndef EDITOR_PANE_H
#define EDITOR_PANE_H

#include <SFML/Graphics.hpp>

class Editor;

class EditorModule
{
protected:

    Editor& editor;
    sf::IntRect bounds;
    float ui_scale = 1.f;
    sf::RectangleShape visible_rect;
    bool mouse_hover;

    // Colors

    const sf::Color C_DEFAULT{50, 50, 50};
    const sf::Color C_HOVER{80, 80, 80};

public:

    EditorModule(Editor& editor);
    virtual void update() {}
    virtual void draw(sf::RenderWindow& window) {}
    void draw_bounds(sf::RenderWindow& window);

    virtual void set_bounds(const sf::IntRect& bounds);
    virtual void set_ui_scale(float new_scale);
    void set_hover_highlight(bool hover);
    sf::IntRect get_bounds();
};

#endif
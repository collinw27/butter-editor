#ifndef EDITOR_PANE_H
#define EDITOR_PANE_H

#include <SFML/Graphics.hpp>

class Editor;

class EditorModule
{
protected:

    // Colors

    const sf::Color C_DEFAULT{50, 50, 50};
    const sf::Color C_HOVER{80, 80, 80};

    Editor& editor;
    sf::IntRect bounds;
    sf::RectangleShape visible_rect;

    bool mouse_hover;

public:

    EditorModule(Editor& editor);
    virtual void update();
    virtual void draw(sf::RenderWindow& window);

    void set_hover_highlight(bool hover);

    void set_bounds(const sf::IntRect& bounds);
    sf::IntRect get_bounds();
};

#endif
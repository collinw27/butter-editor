#ifndef MINI_TERMINAL_H
#define MINI_TERMINAL_H

#include <string>
#include <SFML/Graphics.hpp>

class Editor;

class CommandBar
{
    Editor& editor;
    sf::IntRect bounds;
    sf::Text status_text;
    sf::Text command_text;

    bool typing = false;
    std::string command;

public:

    CommandBar(Editor& editor, sf::IntRect bounds);
    void update();
    void draw(sf::RenderWindow& window);

    bool attempt_clear();
    void set_typing(bool value);
    void set_bounds(const sf::IntRect& bounds);
    void update(std::string keyboard_string);
};

#endif
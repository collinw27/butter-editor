#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "editor/Editor.h"

#include <vector>
#include <SFML/Graphics.hpp>

class Input
{
    static Input *singleton_object;

    std::vector<sf::Keyboard::Key> key_presses;

    void clear_keys();
    void add_key_press(sf::Keyboard::Key key);

public:

    static Input *singleton();
    Input();
    ~Input();

    bool check_key_press(sf::Keyboard::Key key);

    friend void Editor::run();
};

#endif
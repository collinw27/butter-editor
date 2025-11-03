#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <vector>
#include <SFML/Graphics.hpp>

class Input
{
    static Input *singleton_object;

    std::vector<sf::Keyboard::Key> key_holds;
    std::vector<sf::Keyboard::Key> key_presses;

public:

    static Input *singleton();
    Input();
    ~Input();

    void clear_keys();
    void add_key_press(sf::Keyboard::Key key);

    bool check_key(sf::Keyboard::Key key);
    bool check_key_press(sf::Keyboard::Key key);
};

#endif
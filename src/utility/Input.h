#ifndef INPUT_H
#define INPUT_H

#include <vector>
#include <SFML/Graphics.hpp>

// Testing this out for potential use

using SF_KEY = sf::Keyboard::Key;

enum class KeyMod
{
    NONE    = 0,
    CTRL    = 1 << 0,
    SHIFT   = 1 << 1,
    ALT     = 1 << 2
};

// Originally, the plan was to allow space to function like a mouse input
// Some programs let you scroll with the space bar, and this was an attempt
// to replicate that
// Even though this functionality has been removed, mouse events still use a
// separate enum than sf::Mouse::Button to allow behavior like this to be
// potentially re-added without any major refactoring

enum class InputButton
{
    LEFT,
    RIGHT,
    MIDDLE
};

class InputSingleton;

InputSingleton& Input();

class InputSingleton
{
    static InputSingleton* singleton_object;

    std::vector<SF_KEY> key_holds;
    std::vector<SF_KEY> key_presses;

public:

    InputSingleton();
    ~InputSingleton();

    void clear_keys();
    void add_key_press(SF_KEY key);

    bool check_key(SF_KEY key, KeyMod mod = KeyMod::NONE);
    bool check_key_press(SF_KEY key, KeyMod mod = KeyMod::NONE);
    bool check_ctrl();
    bool check_shift();
    bool check_alt();

    bool check_mouse(sf::Mouse::Button button);

    // Singleton returned by external function to make code cleaner

    friend InputSingleton& Input();
};

#endif
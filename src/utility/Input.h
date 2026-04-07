#ifndef InputSingleton_MANAGER_H
#define InputSingleton_MANAGER_H

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

    // Singleton returned by external function to make code cleaner

    friend InputSingleton& Input();
};

#endif
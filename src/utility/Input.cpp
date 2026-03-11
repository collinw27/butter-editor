#include "utility/Input.h"

#include <algorithm>

Input* Input::singleton_object = nullptr;

Input* Input::singleton()
{
    return singleton_object;
}

Input::Input()
{
    Input::singleton_object = this;
}

Input::~Input()
{
    Input::singleton_object = nullptr;
}

void Input::clear_keys()
{
    key_presses.clear();
}

void Input::add_key_press(sf::Keyboard::Key key)
{
    key_presses.push_back(key);
}

bool Input::check_key(sf::Keyboard::Key key, KeyMod mod)
{
    if ((int)mod & (int)KeyMod::CTRL
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)
    ) return false;
    if ((int)mod & (int)KeyMod::SHIFT
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)
    ) return false;
    if ((int)mod & (int)KeyMod::ALT
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RAlt)
    ) return false;
    return sf::Keyboard::isKeyPressed(key);
}

bool Input::check_key_press(sf::Keyboard::Key key, KeyMod mod)
{
    if ((int)mod & (int)KeyMod::CTRL && !check_ctrl()) return false;
    if ((int)mod & (int)KeyMod::SHIFT && !check_shift()) return false;
    if ((int)mod & (int)KeyMod::ALT && !check_alt()) return false;
    return std::find(key_presses.begin(), key_presses.end(), key) != key_presses.end();
}

bool Input::check_ctrl()
{
    return (
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)
    );
}

bool Input::check_shift()
{
    return (
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)
    );
}

bool Input::check_alt()
{
    return (
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RAlt)
    );
}
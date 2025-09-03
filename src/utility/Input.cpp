#include "utility/Input.h"

#include <algorithm>

Input *Input::singleton_object = nullptr;

Input *Input::singleton()
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

bool Input::check_key_press(sf::Keyboard::Key key)
{
    return std::find(key_presses.begin(), key_presses.end(), key) != key_presses.end();
}
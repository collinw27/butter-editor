#include "utility/Input.h"

#include <algorithm>
#include "utility/core.h"

InputSingleton* InputSingleton::singleton_object = nullptr;

InputSingleton& Input()
{
    return *InputSingleton::singleton_object;
}

InputSingleton::InputSingleton()
{
    if (InputSingleton::singleton_object != nullptr)
        throw ButterException("Reinitialization of singleton");
    InputSingleton::singleton_object = this;
}

InputSingleton::~InputSingleton()
{
    InputSingleton::singleton_object = nullptr;
}

void InputSingleton::clear_keys()
{
    key_presses.clear();
}

void InputSingleton::add_key_press(SF_KEY key)
{
    key_presses.push_back(key);
}

bool InputSingleton::check_key(SF_KEY key, KeyMod_t required, KeyMod_t optional)
{
    // Start by creating a bit sequence of pressed function keys
    // Bypassed if key mods are all optional

    if (!(required == KeyMod::NONE && optional == KeyMod::ALL))
    {
        KeyMod_t held_keys = (0
            + (check_ctrl() ? (int) KeyMod::CTRL : 0)
            + (check_shift() ? (int) KeyMod::SHIFT : 0)
            + (check_alt() ? (int) KeyMod::ALT : 0)
        );

        // Fail if required key is not held

        if (held_keys & required != required)
            return false;
        
        // Fail is blocked key is held
        // A key is considered blocked if it's not required nor optional

        KeyMod_t blocked = ~(required | optional);
        if (held_keys & blocked != 0)
            return false;
    }

    return sf::Keyboard::isKeyPressed(key);
}

bool InputSingleton::check_key_press(SF_KEY key, KeyMod_t required, KeyMod_t optional)
{
    // Start by creating a bit sequence of pressed function keys
    // Bypassed if key mods are all optional

    if (!(required == KeyMod::NONE && optional == KeyMod::ALL))
    {
        KeyMod_t held_keys = (0
            + (check_ctrl() ? (int) KeyMod::CTRL : 0)
            + (check_shift() ? (int) KeyMod::SHIFT : 0)
            + (check_alt() ? (int) KeyMod::ALT : 0)
        );

        // Fail if required key is not held

        if ((held_keys & required) != required)
            return false;
        
        // Fail is blocked key is held
        // A key is considered blocked if it's not required nor optional
        // `& KeyMod::ALL` sets the leading digits to 0

        KeyMod_t blocked = ~(required | optional) & KeyMod::ALL;
        if ((held_keys & blocked) != 0)
            return false;
    }

    return std::find(key_presses.begin(), key_presses.end(), key) != key_presses.end();
}

bool InputSingleton::check_ctrl()
{
    return (
        sf::Keyboard::isKeyPressed(SF_KEY::LControl) ||
        sf::Keyboard::isKeyPressed(SF_KEY::RControl)
    );
}

bool InputSingleton::check_shift()
{
    return (
        sf::Keyboard::isKeyPressed(SF_KEY::LShift) ||
        sf::Keyboard::isKeyPressed(SF_KEY::RShift)
    );
}

bool InputSingleton::check_alt()
{
    return (
        sf::Keyboard::isKeyPressed(SF_KEY::LAlt) ||
        sf::Keyboard::isKeyPressed(SF_KEY::RAlt)
    );
}

bool InputSingleton::check_mouse(sf::Mouse::Button button)
{
    return sf::Mouse::isButtonPressed(button);
}
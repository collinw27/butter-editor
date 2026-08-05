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

bool InputSingleton::check_key(SF_KEY key, KeyMod mod)
{
    if ((int)mod & (int)KeyMod::CTRL
        && !sf::Keyboard::isKeyPressed(SF_KEY::LControl)
        && !sf::Keyboard::isKeyPressed(SF_KEY::RControl)
    ) return false;
    if ((int)mod & (int)KeyMod::SHIFT
        && !sf::Keyboard::isKeyPressed(SF_KEY::LShift)
        && !sf::Keyboard::isKeyPressed(SF_KEY::RShift)
    ) return false;
    if ((int)mod & (int)KeyMod::ALT
        && !sf::Keyboard::isKeyPressed(SF_KEY::LAlt)
        && !sf::Keyboard::isKeyPressed(SF_KEY::RAlt)
    ) return false;
    return sf::Keyboard::isKeyPressed(key);
}

bool InputSingleton::check_key_press(SF_KEY key, KeyMod mod)
{
    if ((int)mod & (int)KeyMod::CTRL && !check_ctrl()) return false;
    if ((int)mod & (int)KeyMod::SHIFT && !check_shift()) return false;
    if ((int)mod & (int)KeyMod::ALT && !check_alt()) return false;
    return std::find(key_presses.begin(), key_presses.end(), key) != key_presses.end();
}

bool InputSingleton::check_ctrl()
{
    return (
        sf::Keyboard::isKeyPressed(SF_KEY::LControl)
        || sf::Keyboard::isKeyPressed(SF_KEY::RControl)
    );
}

bool InputSingleton::check_shift()
{
    return (
        sf::Keyboard::isKeyPressed(SF_KEY::LShift)
        || sf::Keyboard::isKeyPressed(SF_KEY::RShift)
    );
}

bool InputSingleton::check_alt()
{
    return (
        sf::Keyboard::isKeyPressed(SF_KEY::LAlt)
        || sf::Keyboard::isKeyPressed(SF_KEY::RAlt)
    );
}

bool InputSingleton::check_mouse(sf::Mouse::Button button)
{
    return sf::Mouse::isButtonPressed(button);
}
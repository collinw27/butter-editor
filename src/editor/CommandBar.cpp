#include "editor/CommandBar.h"

#include "utility/Input.h"
#include "utility/ResourceManager.h"
#include "utility/Logger.h"

CommandBar::CommandBar(Editor &editor, sf::IntRect bounds) :
    editor{editor}, bounds{bounds}, 
    status_text{ResourceManager::singleton()->get_mono(), "Untitled project  |  00h 00m 00s 000ms", 15u},
    command_text{ResourceManager::singleton()->get_mono(), "> ", 15u}
{
    command = "";
    status_text.setPosition(sf::Vector2f(bounds.position) + sf::Vector2f(5, 0));
    command_text.setPosition(sf::Vector2f(bounds.position) + sf::Vector2f(5, 0));
}

void CommandBar::update() {}

void CommandBar::draw(sf::RenderWindow& window)
{
    window.draw(typing ? command_text : status_text);
}

// Returns whether anything was cleared

bool CommandBar::attempt_clear()
{
    if (command.empty())
        return false;
    command = "";
    command_text.setString("> ");
    return true;
}

void CommandBar::set_typing(bool value)
{
    typing = value;
}

void CommandBar::set_bounds(const sf::IntRect& bounds)
{
    this->bounds = bounds;
    status_text.setPosition(sf::Vector2f(bounds.position) + sf::Vector2f(5, 0));
    command_text.setPosition(sf::Vector2f(bounds.position) + sf::Vector2f(5, 0));
}

void CommandBar::update(std::string typed_string)
{
    if (!typed_string.empty())
    {
        // Ignore control characters (ASCII < 32)

        for (auto c : typed_string)
        {
            if ((int)c >= 32)
                command += c;
        }
        command_text.setString("> " + command);
    }
    if (Input::singleton()->check_key_press(sf::Keyboard::Key::Backspace))
    {
        if (!command.empty())
            command = command.substr(0, command.length() - 1);
        command_text.setString("> " + command);
    }
}
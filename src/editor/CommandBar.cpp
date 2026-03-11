#include "editor/CommandBar.h"

#include <cmath>
#include "editor/Editor.h"
#include "utility/Math.h"
#include "utility/Input.h"
#include "utility/ResourceManager.h"
#include "utility/Logger.h"

constexpr int CHAR_LIMIT = 1024;

CommandBar::CommandBar(Editor &editor, sf::IntRect bounds) :
    editor{editor}, bounds{bounds}, 
    status_text{ResourceManager::singleton()->get_mono(), "Untitled project  |  00h 00m 00s 000ms", 15u},
    command_text{ResourceManager::singleton()->get_mono(), "> ", 15u}
{
    command = "";
    status_text.setPosition(sf::Vector2f(bounds.position) + sf::Vector2f(5, 0));
    command_text.setPosition(sf::Vector2f(bounds.position) + sf::Vector2f(5, 0));

    cursor_rect = sf::RectangleShape(sf::Vector2f(2, 22));
    cursor_rect.setPosition(sf::Vector2f(bounds.position));
    cursor_rect.setFillColor(sf::Color::White);

    selection_rect.setFillColor(sf::Color(90, 90, 90));
}

void CommandBar::update(const std::string& typed_string)
{
    // Check for special keys

    bool pressed_l = Input::singleton()->check_key_press(SF_KEY::Left);
    bool pressed_r = Input::singleton()->check_key_press(SF_KEY::Right);
    bool pressed_home = Input::singleton()->check_key_press(SF_KEY::Home);
    bool pressed_end = Input::singleton()->check_key_press(SF_KEY::End);
    if (Input::singleton()->check_key_press(sf::Keyboard::Key::Backspace))
        backspace(Input::singleton()->check_ctrl());
    if (Input::singleton()->check_key_press(sf::Keyboard::Key::Delete))
        delete_ahead(Input::singleton()->check_ctrl());
    if (pressed_l || pressed_r)
        move_cursor(pressed_r, (Input::singleton()->check_ctrl() ? MoveMode::WORD : MoveMode::ONE), Input::singleton()->check_shift());
    if (pressed_home || pressed_end)
        move_cursor(pressed_end, MoveMode::ALL, Input::singleton()->check_shift());
    if (Input::singleton()->check_key_press(SF_KEY::A, KeyMod::CTRL))
        (Input::singleton()->check_shift() ? reset_selection() : select_all());

    // Now, create normal text

    if (!Input::singleton()->check_ctrl())
        append(typed_string);

    // Update visuals

    int dummy = 1;
    command_text.setString("> " + command);
    float start_x = command_text.findCharacterPos(2 + cursor_start).x;
    cursor_rect.setPosition(sf::Vector2f(bounds.position) + sf::Vector2f(start_x, -2));
    if (cursor_end != -1)
    {
        float end_x = command_text.findCharacterPos(2 + cursor_end).x;
        selection_rect.setPosition(sf::Vector2f(bounds.position) + sf::Vector2f(std::min(start_x, end_x), -2));
        selection_rect.setSize(sf::Vector2f(std::max(start_x, end_x) - std::min(start_x, end_x), 22));
    }
    cursor_time += editor.get_delta_time();
}

void CommandBar::draw(sf::RenderWindow& window)
{
    if (typing)
    {
        if (cursor_end != -1)
            window.draw(selection_rect);
        window.draw(command_text);
        if ((int)std::floor(cursor_time / 0.7) % 2 == 0)
            window.draw(cursor_rect);
    }
    else
    {
        window.draw(status_text);
    }
}

// Returns whether anything was cleared

bool CommandBar::attempt_clear()
{
    if (command.empty())
        return false;
    command = "";
    cursor_start = 0;
    cursor_end = -1;
    cursor_time = 0.f;
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

void CommandBar::append(const std::string& raw_text)
{
    // Filter the new text, ignoring control characters (ASCII < 32)

    std::string new_text = "";
    for (auto c : raw_text)
    {
        if ((int)c >= 32)
            new_text += c;
    }

    if (!new_text.empty())
    {
        // Remove any selected text
        
        if (cursor_end != -1)
        remove_selected_chars();
        
        // Add the text

        command = command.substr(0, cursor_start) + new_text + command.substr(cursor_start);
        if (command.length() > CHAR_LIMIT)
            command = command.substr(0, CHAR_LIMIT);
        cursor_start = std::min(cursor_start + new_text.length(), command.length());
        cursor_time = 0.f;
    }
}

// For more complex deletions, simulates selecting text to avoid code duplication
// Don't tell anyone

void CommandBar::backspace(bool delete_word)
{
    if (cursor_end != -1)
        remove_selected_chars();
    else if (cursor_start > 0)
    {
        if (!delete_word)
        {
            command = command.substr(0, cursor_start - 1) + command.substr(cursor_start);
            --cursor_start;
        }
        else
        {
            move_cursor(false, MoveMode::WORD, true);
            remove_selected_chars();
        }
        cursor_time = 0.f;
    }
}

void CommandBar::delete_ahead(bool delete_word)
{
    if (cursor_end != -1)
        remove_selected_chars();
    else if (cursor_start < command.length())
    {
        if (!delete_word)
        {
            command = command.substr(0, cursor_start) + command.substr(cursor_start + 1);
        }
        else
        {
            move_cursor(true, MoveMode::WORD, true);
            remove_selected_chars();
        }
        cursor_time = 0.f;
    }
}

void CommandBar::remove_selected_chars()
{
    int start = std::min(cursor_start, cursor_end);
    int end = std::max(cursor_start, cursor_end);
    command = command.substr(0, start) + command.substr(end);
    cursor_start = start;
    cursor_end = -1;
    cursor_time = 0.f;
}

void CommandBar::move_cursor(bool forward, MoveMode mode, bool do_select)
{
    // Start selecting if necessary
    // If shift is not held, any selection will be deselected

    if (do_select && cursor_end == -1)
    {
        cursor_end = cursor_start;
    }
    else if (!do_select && cursor_end != -1)
    {
        reset_selection();
    }
    if (mode == MoveMode::ONE)
    {
        cursor_start = clamp(cursor_start + (forward ? 1 : -1), 0, (int)command.length());
    }
    else if (mode == MoveMode::WORD)
    {
        bool checking = false;
        while (!(forward && cursor_start == command.length()) && !(!forward && cursor_start == 0))
        {
            // Stop before the first space after the first non-space character

            cursor_start += (forward ? 1 : -1);
            if (command.at(cursor_start + (forward ? -1 : 0)) != ' ')
                checking = true;
            if (checking && cursor_start != 0 && cursor_start < command.length() && command.at(cursor_start + (forward ? 0 : -1)) == ' ')
                break;
        }
    }
    else if (mode == MoveMode::ALL)
    {
        cursor_start = (forward ? command.length() : 0);
    }

    // If this was a no-op, then no selection should remain
    
    if (cursor_start == cursor_end)
        reset_selection();
    cursor_time = 0.f;
}

void CommandBar::reset_selection()
{
    cursor_end = -1;
}

void CommandBar::select_all()
{
    cursor_time = 0.f;
    cursor_end = 0;
    cursor_start = command.length();
}
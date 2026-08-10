#include "editor/command_bar/CommandBar.h"

#include <cmath>
#include "editor/Editor.h"
#include "utility/core.h"
#include "utility/Input.h"
#include "utility/FileManager.h"
#include "utility/Graphics.h"
#include "command/CommandParser.h"

constexpr int CHAR_LIMIT = 1024;
constexpr int HISTORY_LIMIT = 64;

// Bounds & ui_scale are not initialized until respective setters are called (by Editor)

CommandBar::CommandBar(Editor &editor) :
    editor{editor}
{
    command = "";
    
    container.reset(GLContainer::create(nullptr, sf::Vector2f(bounds.position), sf::Vector2f(1000, 1000)));
    selection_rect.reset(GLRectangle::create(container.get()));
    selection_rect->set_fill_color(Editor::C_HIGHLIGHT);
    status_text.reset(GLText::create(container.get(), Graphics().mono_font(), 0u, ""));
    command_text.reset(GLText::create(container.get(), Graphics().mono_font(), 0u, "> "));
    cursor_rect.reset(GLRectangle::create(container.get()));
    cursor_rect->set_fill_color(sf::Color::White);

    command_text->enable_special_formatting();
    command_text->set_visible(false);
    cursor_rect->set_visible(false);
    selection_rect->set_visible(false);
}

void CommandBar::update(const std::string& typed_string)
{
    // Modifying the text removes the red highlight

    std::string prev_command = command;
        
    // Check for special keys

    bool pressed_l = Input().check_key_press(SF_KEY::Left, KeyMod::NONE, KeyMod::ALL);
    bool pressed_r = Input().check_key_press(SF_KEY::Right, KeyMod::NONE, KeyMod::ALL);
    bool pressed_home = Input().check_key_press(SF_KEY::Home, KeyMod::NONE, KeyMod::ALL);
    bool pressed_end = Input().check_key_press(SF_KEY::End, KeyMod::NONE, KeyMod::ALL);
    if (Input().check_key_press(SF_KEY::Backspace, KeyMod::NONE, KeyMod::ALL))
        backspace(Input().check_ctrl());
    if (Input().check_key_press(SF_KEY::Delete, KeyMod::NONE, KeyMod::ALL))
        delete_ahead(Input().check_ctrl());
    if (pressed_l || pressed_r)
        move_cursor(pressed_r, (Input().check_ctrl() ? MoveMode::WORD : MoveMode::ONE), Input().check_shift());
    if (pressed_home || pressed_end)
        move_cursor(pressed_end, MoveMode::ALL, Input().check_shift());
    if (Input().check_key_press(SF_KEY::C, KeyMod::CTRL) && cursor_end != -1)
        copy();
    if (Input().check_key_press(SF_KEY::X, KeyMod::CTRL) && cursor_end != -1)
    {
        copy();
        remove_selected_chars();
    }
    if (Input().check_key_press(SF_KEY::V, KeyMod::CTRL))
        paste();
    if (Input().check_key_press(SF_KEY::A, KeyMod::CTRL, KeyMod::SHIFT))
        (Input().check_shift() ? reset_selection() : select_all());
    if (Input().check_key_press(SF_KEY::Up))
        traverse_history(1);
    if (Input().check_key_press(SF_KEY::Down))
        traverse_history(-1);

    // Now, create normal text

    if (!Input().check_ctrl())
        append(typed_string);

    // Update visuals

    invalid_command = invalid_command && (prev_command == command);
    if (text_updated)
        render_text();
    text_updated = false;
    cursor_time += editor.get_delta_time();
}

GLNode* CommandBar::get_node()
{
    return container.get();
}

void CommandBar::set_bounds(const sf::IntRect& new_bounds)
{
    bounds = new_bounds;
    container->set_position(sf::Vector2f(bounds.position));
    container->set_size(sf::Vector2f(bounds.size));
    status_text->set_position(sf::Vector2f(5, 0));
    command_text->set_position(sf::Vector2f(5, 0));
}

void CommandBar::set_ui_scale(float new_scale)
{
    ui_scale = new_scale;
    float text_size = (unsigned int)(16.f * ui_scale);
    status_text->set_char_size(text_size);
    command_text->set_char_size(text_size);
    cursor_rect->set_size(sf::Vector2f(2, text_size + 7));
    render_text();
}

void CommandBar::set_status_name(std::string name)
{
    status_name = name;
    refresh_status();
}

void CommandBar::set_status_length(std::string length)
{
    status_length = length;
    refresh_status();
}

void CommandBar::set_status_exporting(std::string percentage)
{
    status_exporting = percentage;
    refresh_status();
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
    invalid_command = false;
    text_updated = true;
    render_text();
    return true;
}

// Returns an invalid result if command could not be parsed
// Otherwise, returns the parsed command
// An empty commands indicates the command bar should be exited

void CommandBar::submit_valid()
{
    // Log the result

    auto duplicate = std::find(history.begin(), history.end(), command);
    if (duplicate != history.end())
        history.erase(duplicate);
    history.insert(history.begin(), command);
    if (history.size() > HISTORY_LIMIT)
        history.erase(history.end() - 1);
    
    // Clear terminal
    
    invalid_command = false;
    attempt_clear();
    history_index = -1;
}

void CommandBar::submit_error()
{
    invalid_command = true;
    text_updated = true;
}

void CommandBar::set_typing(bool value)
{
    typing = value;
    
    status_text->set_visible(!typing);
    command_text->set_visible(typing);
    cursor_rect->set_visible(typing);
    selection_rect->set_visible(typing);
    if (typing)
        render_text();
}

std::string CommandBar::get_command()
{
    return command;
}

void CommandBar::refresh_status()
{
    std::string status_str = status_name + "  |  " + status_length;
    if (status_exporting != "")
        status_str += "  |  " + status_exporting;
    status_text->set_string(status_str);
}

void CommandBar::append(const std::string& raw_text)
{
    // Filter the new text, ignoring control characters (ASCII < 32)

    std::string new_text = "";
    for (auto c : raw_text)
    {
        if ((int) c >= 32)
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
    text_updated = true;
}

// Don't tell anyone, but for more complex deletions, it
// reuses the selection routine to simulate selecting and deleting

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
    text_updated = true;
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
    text_updated = true;
}

void CommandBar::remove_selected_chars()
{
    int start = std::min(cursor_start, cursor_end);
    int end = std::max(cursor_start, cursor_end);
    command = command.substr(0, start) + command.substr(end);
    cursor_start = start;
    cursor_end = -1;
    cursor_time = 0.f;
    text_updated = true;
}

void CommandBar::copy()
{
    if (cursor_end == -1)
        return;
    int start = std::min(cursor_start, cursor_end);
    int end = std::max(cursor_start, cursor_end);
    std::string selection = command.substr(start, end - start);
    sf::Clipboard::setString(selection);
}

void CommandBar::paste()
{
    append(sf::Clipboard::getString());
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

    // Cancel selection if no characters are selected
    
    if (cursor_start == cursor_end)
        reset_selection();
    cursor_time = 0.f;
    text_updated = true;
}

void CommandBar::reset_selection()
{
    cursor_end = -1;
    text_updated = true;
}

void CommandBar::select_all()
{
    cursor_time = 0.f;
    cursor_end = 0;
    cursor_start = command.length();
    text_updated = true;
}

void CommandBar::traverse_history(int delta)
{
    int old_index = history_index;
    history_index = clamp<int>(history_index + delta, -1, history.size() - 1);
    if (history_index != old_index)
    {
        attempt_clear();
        if (history_index != -1)
            append(history.at(history_index));
    }
}

void CommandBar::render_text()
{
    CommandParser& parser = editor.get_command_parser();
    CommandStructure structure = parser.parse_structure(command);

    // In the case that there is an error in the command, text formatting is simple

    command_text->reset_formatting();
    command_text->set_string("> " + command);
    if ((invalid_command || !structure.valid()) && !command.empty())
    {
        command_text->add_color(Editor::C_INVALID, 0);
    }

    // Otherwise, color breakpoints are added based on argument type

    else
    {
        for (int i = 0; i < structure.get_size(); ++i)
        {
            CommandStructure::TokenType this_type = structure.get_token_type(i);
            sf::Color this_color = sf::Color::White;
            switch (this_type)
            {
            case CommandStructure::TokenType::BOOL:
                this_color = sf::Color(255, 239, 115);
            break;
            case CommandStructure::TokenType::INT:
                this_color = sf::Color(122, 246, 255);
            break;
            case CommandStructure::TokenType::FLOAT:
                this_color = sf::Color(249, 135, 255);
            break;
            case CommandStructure::TokenType::STRING:
                this_color = sf::Color(107, 255, 127);
            break;
            case CommandStructure::TokenType::INVALID:
                this_color = Editor::C_INVALID;
            break;
            }
            command_text->add_color(this_color, structure.get_token_start(i) + 2);
        }
        
        // Hints are shown for remaining arguments

        if (structure.get_size() >= 1)
        {
            command_text->add_color(Editor::C_HIGHLIGHT_SUBTLE);
            if (command_text->get_string().back() != ' ')
                command_text->add_string(" ");
            std::string root = structure.get_token(0);
            for (int i = structure.get_size() - 1; i < parser.get_param_count(root); ++i)
            {
                std::string p_name = parser.get_param_name(root, i);
                command_text->add_string("<" + p_name + "> ");
            }
        }
    }

    // Move cursor

    float start_x = command_text->find_char_pos(2 + cursor_start).x;
    cursor_rect->set_position(command_text->get_position() + sf::Vector2f(start_x, -2 + ui_scale));

    // Two situations in which a highlight is shown:
    // 1) User has highlighted text
    // 2) Used to indicate range of current argument when not highlighted
    // Only one of these can be present at once, and it's possible for neither
    // to be present
    
    if (cursor_end != -1)
    {
        float end_x = command_text->find_char_pos(2 + cursor_end).x;
        selection_rect->set_position(command_text->get_position() + sf::Vector2f(std::min(start_x, end_x), -2 + ui_scale));
        selection_rect->set_size(sf::Vector2f(std::max(start_x, end_x) - std::min(start_x, end_x), cursor_rect->get_size().y));
        selection_rect->set_visible(true);
        selection_rect->set_fill_color(Editor::C_HIGHLIGHT);
    }
    else if (structure.valid())
    {
        // Find if there's a token we're inside of

        selection_rect->set_visible(false);
        for (int i = 0; i < structure.get_size(); ++i)
        {
            if (structure.get_token_start(i) <= cursor_start &&
                structure.get_token_start(i) + structure.get_token(i).length() >= cursor_start
            )
            {
                start_x = command_text->find_char_pos(2 + structure.get_token_start(i)).x;
                float end_x = command_text->find_char_pos(2 + structure.get_token_start(i) + structure.get_token(i).length()).x;
                selection_rect->set_position(command_text->get_position() + sf::Vector2f(std::min(start_x, end_x), -2 + ui_scale));
                selection_rect->set_size(sf::Vector2f(std::max(start_x, end_x) - std::min(start_x, end_x), cursor_rect->get_size().y));
                selection_rect->set_visible(true);
                selection_rect->set_fill_color(Editor::C_HIGHLIGHT_SUBTLE);
                break;
            }
        }
    }
    else
    {
        selection_rect->set_visible(false);
    }
}
#ifndef COMMAND_BAR_H
#define COMMAND_BAR_H

#include <string>
#include "graphics/nodes.h"
#include "command/CommandParser.h"

class Editor;

class CommandBar
{
    Editor& editor;
    sf::IntRect bounds;
    float ui_scale = 1.f;
    GLContainer* container;
    GLRectangle* cursor_rect;
    GLRectangle* selection_rect;
    GLText* status_text;
    GLText* command_text;

    // Note: `cursor_start` isn't necessarily less than `cursor_end`

    bool typing = false;
    std::string command;
    int cursor_start = 0;
    int cursor_end = -1;
    float cursor_time = 0.f;

    // Used for highlighting text red when an invalid command is submitted

    bool invalid_command = false;

    // Used for optimization (not yet actually)

    bool textNeedsUpdate = false;

    // For now, the command bar is responsible for parsing & defining commands
    // This will likely be moved to a separate class in the future

    CommandParser command_parser;

public:

    CommandBar(Editor& editor);
    void update(const std::string& keyboard_string);
    GLNode* get_node();
    
    void set_bounds(const sf::IntRect& new_bounds);
    void set_ui_scale(float new_scale);

    bool attempt_clear();
    CommandResult attempt_submit();
    void set_typing(bool value);
    
private:

    // Functions to make the update loop cleaner

    enum class MoveMode { ONE, WORD, ALL };

    void append(const std::string& new_text);
    inline void backspace(bool delete_word);
    inline void delete_ahead(bool delete_word);
    void remove_selected_chars();
    void move_cursor(bool forward, MoveMode mode, bool do_select);
    inline void reset_selection();
    void select_all();

    void render_text();
};

#endif
#ifndef COMMAND_BAR_H
#define COMMAND_BAR_H

#include <string>
#include <vector>
#include "graphics/nodes.h"

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

    // Status text information

    std::string status_name;
    std::string status_length;
    std::string status_exporting = "";

    // Note: `cursor_start` isn't necessarily less than `cursor_end`

    bool typing = false;
    std::string command;

    // Traversal

    int cursor_start = 0;
    int cursor_end = -1;
    float cursor_time = 0.f;
    std::vector<std::string> history {};
    int history_index = -1;

    // Used for highlighting text red when an invalid command is submitted

    bool invalid_command = false;

    // Text graphics are only updated when necessary

    bool text_updated = true;

public:

    CommandBar(Editor& editor);
    void update(const std::string& keyboard_string);
    GLNode* get_node();
    
    void set_bounds(const sf::IntRect& new_bounds);
    void set_ui_scale(float new_scale);

    void set_status_name(std::string name);
    void set_status_length(std::string length);
    void set_status_exporting(std::string percentage);

    bool attempt_clear();
    void submit_valid();
    void submit_error();
    void set_typing(bool value);
    std::string get_command();
    
private:

    // Functions to make the update loop cleaner

    enum class MoveMode { ONE, WORD, ALL };

    void refresh_status();

    void append(const std::string& new_text);
    inline void backspace(bool delete_word);
    inline void delete_ahead(bool delete_word);
    void remove_selected_chars();
    void copy();
    void paste();
    void move_cursor(bool forward, MoveMode mode, bool do_select);
    inline void reset_selection();
    void select_all();
    void traverse_history(int delta);

    void render_text();
};

#endif
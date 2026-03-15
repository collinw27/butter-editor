#ifndef MINI_TERMINAL_H
#define MINI_TERMINAL_H

#include <string>
#include <SFML/Graphics.hpp>

class Editor;

class CommandBar
{
    Editor& editor;
    sf::IntRect bounds;
    float ui_scale = 1.f;
    sf::RectangleShape cursor_rect;
    sf::RectangleShape selection_rect;

    // Some members use pointers to avoid need for direct initialization
    
    sf::Text* status_text;
    sf::Text* command_text;

    // Note: `cursor_start` isn't necessarily less than `cursor_end`

    bool typing = false;
    std::string command;
    int cursor_start = 0;
    int cursor_end = -1;
    float cursor_time = 0.f;

    // Used for optimization (not yet actually)

    bool textNeedsUpdate = false;

public:

    CommandBar(Editor& editor);
    void update(const std::string& keyboard_string);
    void draw(sf::RenderWindow& window);
    
    void set_bounds(const sf::IntRect& bounds);
    void set_ui_scale(float new_scale);

    bool attempt_clear();
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
#ifndef MINI_TERMINAL_H
#define MINI_TERMINAL_H

#include <string>
#include <SFML/Graphics.hpp>

class Editor;

class CommandBar
{
    Editor& editor;
    sf::IntRect bounds;
    sf::Text status_text;
    sf::Text command_text;
    sf::RectangleShape cursor_rect;
    sf::RectangleShape selection_rect;

    // Note: `cursor_start` isn't necessarily less than `cursor_end`

    bool typing = false;
    std::string command;
    int cursor_start = 0;
    int cursor_end = -1;
    float cursor_time = 0.f;

    bool textNeedsUpdate = false; // Used for optimization

public:

    CommandBar(Editor& editor, sf::IntRect bounds);
    void update(const std::string& keyboard_string);
    void draw(sf::RenderWindow& window);

    bool attempt_clear();
    void set_typing(bool value);
    void set_bounds(const sf::IntRect& bounds);
    
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
};

#endif
#ifndef EDITOR_H
#define EDITOR_H

#include <SFML/Graphics.hpp>
#include <vector>

#include "editor/EditorModule.h"
#include "editor/DragMouse.h"

class Editor
{
    sf::RenderWindow *window;

    EditorModule *preview_module;
    EditorModule *config_module;
    EditorModule *timeline_module;
    std::vector<EditorModule*> modules;

    sf::Vector2i window_size;
    sf::Vector2i mouse_position;
    DragMouse* drag_mouse_event = nullptr;

    // For now, the editor is divided into 3 windows
    // This may have more customization in the far future, but for now,
    // this layout is hard-coded into the Editor

    int y_divider;
    int x_divider;

public:

    Editor();
    ~Editor();
    void run();

    void set_cursor(sf::Cursor::Type cursor_type);

private:

    void draw(sf::RenderWindow& window);

    void on_resized(sf::Vector2i new_size);
    void on_mouse_moved(sf::Vector2i position);
    void on_mouse_pressed();
    void on_mouse_released();
    sf::Vector2i get_mouse_position();

    void resize_modules();
};

#endif
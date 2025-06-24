#ifndef EDITOR_H
#define EDITOR_H

#include <SFML/Graphics.hpp>
#include <vector>

#include "editor/EditorModule.h"
#include "editor/config/ConfigModule.h"
#include "editor/config/TimelineModule.h"
#include "editor/DragMouse.h"

class Editor
{
    EditorModule *preview_module;
    EditorModule *config_module;
    EditorModule *timeline_module;
    std::vector<EditorModule*> modules;

    sf::Vector2i mouse_position;
    DragMouse *drag_mouse_event = nullptr;

public:

    Editor();
    ~Editor();
    void draw(sf::RenderWindow& window);

    void on_mouse_moved(sf::Vector2i position);
    void on_mouse_pressed();
    void on_mouse_released();
    sf::Vector2i get_mouse_position();
    void new_mouse_event(DragMouse *new_event);
};

#endif
#ifndef EDITOR_H
#define EDITOR_H

#include <SFML/Graphics.hpp>
#include <vector>

#include "graphics/nodes.h"
#include "editor/EditorModule.h"
#include "editor/core/FlexTab.h"
#include "editor/core/CommandBar.h"
#include "editor/core/DragMouse.h"
#include "editor/core/LogModule.h"
#include "editor/core/ProjectModule.h"
#include "editor/core/TimelineModule.h"
#include "command/CommandParser.h"
#include "project/Project.h"

class Editor
{
public:

    // Start by defining publicly-accessible constants
    // These are defined as static so you don't need access to THE editor instance
    // This could be changed in the future, for example, if color customization is added
    
    static const sf::Color C_BG;
    static const sf::Color C_FG;
    static const sf::Color C_HOVER;
    static const sf::Color C_HIGHLIGHT;
    static const sf::Color C_HIGHLIGHT_SUBTLE;
    static const sf::Color C_FG_DESELECTED;
    static const sf::Color C_INVALID;

private:

    sf::RenderWindow* window;
    GLRootNode* root;

    // For now, the editor has 3 modules visible at all times
    // Flex modules can be toggled between each other using
    // a list of tabs that appear over it
    // `visible_modules` holds the address of the module variable
    // This allows the actual module each variable references to be
    // switched out without having to update `visible_modules`

    EditorModule* preview_module;
    EditorModule* flex_module;
    TimelineModule* timeline_module;
    std::vector<EditorModule**> visible_modules;
    LogModule* log_module;
    ProjectModule* project_module;

    // Command processing
    
    CommandParser command_parser;

    // Top and bottom HUD

    CommandBar* command_bar;
    std::vector<FlexTab*> flex_tabs;
    unsigned int current_flex_tab;
    GLContainer* temp_menu_bar;
    GLText* menu_bar_text;

    sf::Vector2i window_size;
    sf::Vector2i mouse_position;
    DragMouse* drag_mouse_event = nullptr;
    bool using_terminal = false;

    int ui_scale_index = 0;
    float ui_scale = 1.f;

    sf::Clock clock;
    float delta_time = 0.1f;

    // For now, the editor is divided into 3 windows
    // This may have more customization in the far future, but for now,
    // this layout is hard-coded into the Editor

    int y_divider;
    int x_divider;

    // Project
    // See Project.h for explanation of project "locking"

    LockedProject* locked_project;
    Project* project;

    // Special information for asynchronous export task

    bool exporting = false;

public:

    Editor();
    ~Editor();
    void run();

    void set_cursor(sf::Cursor::Type cursor_type);
    float get_delta_time();

    CommandParser& get_command_parser();
    std::string run_command(std::string command, bool throw_errors = false);

    Project* get_project();
    LockedProject* get_locked_project();

private:

    void on_resized(sf::Vector2i new_size);
    void on_mouse_moved(sf::Vector2i position);
    void on_mouse_pressed();
    void on_mouse_released();
    sf::Vector2i get_mouse_position();

    void resize_modules();

    void on_timeline_update();

    void create_project(Project* new_project);
    void lock_project();
    void unlock_project();
    
    void initialize_commands();
    std::string execute_command(CommandResult command);
};

#endif
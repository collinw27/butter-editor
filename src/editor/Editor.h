#ifndef EDITOR_H
#define EDITOR_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

#include "utility/Input.h"
#include "graphics/nodes.h"
#include "editor/core/EditorModule.h"
#include "editor/core/FlexTab.h"
#include "command/CommandParser.h"
#include "project/Project.h"

#include "editor/command_bar/CommandBar.h"
#include "editor/core/DragMouse.h"
#include "editor/timeline/TimelineModule.h"
#include "editor/log/LogModule.h"
#include "editor/media/MediaModule.h"
#include "editor/project/ProjectModule.h"
#include "editor/debug/DebugModule.h"

class Editor
{
public:

    // Start by defining publicly-accessible constants
    // These are defined as static so you don't need access to THE editor instance
    // This could be changed in the future, for example, if color customization is added
    
    static const sf::Color C_BG;
    static const sf::Color C_FG;
    static const sf::Color C_HOVER;
    static const sf::Color C_FOCUSED;
    static const sf::Color C_FOCUSED_HOVER;
    static const sf::Color C_HIGHLIGHT;
    static const sf::Color C_HIGHLIGHT_SUBTLE;
    static const sf::Color C_FG_DESELECTED;
    static const sf::Color C_INVALID;
    static const sf::Color C_SCROLL_STILL;
    static const sf::Color C_SCROLL_HOVER;
    static const sf::Color C_SCROLL_DRAG;

private:

    sf::RenderWindow* window;
    std::unique_ptr<GLRootNode> root;

    // `visible_modules` holds the address of the module variable
    // This allows the actual module each variable references to be
    // switched out without having to update `visible_modules`
    
    std::vector<EditorModule**> visible_modules;

    // Only one module can be "focused" at once
    // (Not to be confused with the `focused` argument in mouse events)
    // This disambiguates which module should respond to key events

    EditorModule* focused_module = nullptr;

    // For now, the editor has 3 modules visible at all times
    // Flex modules can be toggled between each other using
    // a list of tabs that appear over it

    EditorModule* preview_module;
    EditorModule* flex_module;
    TimelineModule* timeline_module;
    LogModule* log_module;
    MediaModule* media_module;
    ProjectModule* project_module;
    DebugModule* debug_module;

    // Command processing
    
    CommandParser command_parser;

    // Top and bottom HUD

    CommandBar* command_bar;
    std::vector<std::unique_ptr<FlexTab>> flex_tabs;
    unsigned int current_flex_tab;
    std::unique_ptr<GLContainer> temp_menu_bar;
    std::unique_ptr<GLText> menu_bar_text;
    sf::Vector2i mouse_position;
    bool using_terminal = false;

    // For now, the editor is divided into 3 windows
    // This may have more customization in the far future, but for now,
    // this layout is hard-coded into the Editor

    int y_divider;
    int x_divider;

    // Mouse events

    std::unique_ptr<DragMouse> drag_mouse_event = nullptr;

    // Window resizing

    sf::Vector2i window_size;
    int ui_scale_index = 0;
    float ui_scale = 1.f;

    // Time

    sf::Clock clock;
    float delta_time = 0.1f;

    // Project
    // See Project.h for explanation of project "locking"
    // Project must be initialized before all modules
    // `project` MUST NOT be stored persistently in any other classes!

    LockedProject* locked_project = nullptr;
    Project* project = nullptr;

    // Special information for asynchronous export task

    bool exporting = false;

public:

    Editor();
    ~Editor();
    void run();

    float get_delta_time();

    void set_cursor(sf::Cursor::Type cursor_type);
    bool set_drag_event(std::unique_ptr<DragMouse> new_event);

    CommandParser& get_command_parser();
    std::string run_command(std::string command, bool throw_errors = false);

    Project* get_project();
    LockedProject* get_locked_project();

private:

    void on_resized(sf::Vector2i new_size);
    void on_mouse_move(sf::Vector2i position);
    void on_mouse_press(InputButton button);
    void on_mouse_release(InputButton button);
    sf::Vector2i get_mouse_position();

    void resize_modules();

    void switch_flex_tab(unsigned int index);

    void on_timeline_update();

    void create_project(Project* new_project);
    void lock_project();
    void unlock_project();
    
    void initialize_commands();
    std::string execute_command(CommandResult command);
};

#endif
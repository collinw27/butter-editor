#include "editor/Editor.h"

#include <sstream>
#include <optional>

#include "utility/core.h"
#include "utility/Graphics.h"
#include "utility/Input.h"
#include "utility/Logger.h"
#include "utility/FileManager.h"
#include "utility/file_formats/UserSettings.h"
#include "utility/Debugger.h"
#include "editor/notifs.h"
#include "editor/core/mouse/DragDivider.h"
#include "command/exceptions.h"

constexpr int MODULE_MARGIN = 100;
constexpr int TAB_HEIGHT = 30;
constexpr int TAB_WIDTH = 90;
constexpr int C_BAR_HEIGHT = 20;

const sf::Color Editor::C_BG_EMPTY {5, 5, 5};
const sf::Color Editor::C_BG {20, 20, 20};
const sf::Color Editor::C_BORDER {90, 90, 90};
const sf::Color Editor::C_BORDER_HOVER {130, 130, 130};
const sf::Color Editor::C_FOCUSED {160, 174, 186};
const sf::Color Editor::C_FOCUSED_HOVER {182, 198, 212};
const sf::Color Editor::C_HIGHLIGHT {90, 90, 90};
const sf::Color Editor::C_HIGHLIGHT_SUBTLE {50, 50, 50};
const sf::Color Editor::C_TAB {90, 90, 90};
const sf::Color Editor::C_TAB_DESELECTED {50, 50, 50};
const sf::Color Editor::C_TAB_HOVER {130, 130, 130};
const sf::Color Editor::C_INVALID {255, 120, 120};
const sf::Color Editor::C_SCROLL_STILL {255, 255, 255, 60};
const sf::Color Editor::C_SCROLL_HOVER {255, 255, 255, 110};
const sf::Color Editor::C_SCROLL_DRAG {255, 255, 255, 180};

Editor::Editor()
{
    // SFML setup
    
    Graphics().init(sf::VideoMode({1280, 720}), "Butter Video Editor", sf::Style::Close | sf::Style::Resize | sf::Style::Titlebar);
    Graphics().set_clear_color(Editor::C_BG_EMPTY);
    window = &Graphics().get_window();
    window_size = sf::Vector2i(window->getSize());
    window->setMinimumSize(sf::Vector2u(300, 200));
    window->setFramerateLimit(150);
    root.reset(GLRootNode::create());
    clock = sf::Clock();
    clock.start();

    // Command setup

    initialize_commands();

    // UI parameters
    // `resize_modules()` must ALWAYS be called, since module properties
    // are dependent on calculations whose implementation are only provided in said function
    // Basic UI parameters are preserved across projects, so this takes place outside of `load_project()`
    
    y_divider = 360;
    x_divider = 640;
    ui_scale_index = FileManager().get_user_settings().ui_scale_index;
    ui_scale = 1.f + (float)ui_scale_index * 0.1f;

    // Create default project
    // Loading a project resets most editor properties, so this step
    // is where most initialization happens

    load_project(new Project(*this));
}

Editor::~Editor()
{
    delete preview_module;
    delete timeline_module;
    delete log_module;
    delete media_module;
    delete project_module;
    delete debug_module;
    delete command_bar;
}

void Editor::run()
{
    while (window->isOpen())
    {
        Input().clear_keys();
        std::string keyboard_string = "";

        // Handle all events

        while (const std::optional event = window->pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window->close();
            }
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                on_resized(sf::Vector2i(resized->size));
            }
            else if (const auto* mouse_moved = event->getIf<sf::Event::MouseMoved>())
            {
                on_mouse_move(mouse_moved->position);
            }
            else if (const auto* mouse_clicked = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse_clicked->button == sf::Mouse::Button::Left)
                    on_mouse_press(InputButton::LEFT);
                else if (mouse_clicked->button == sf::Mouse::Button::Right)
                    on_mouse_press(InputButton::RIGHT);
                else if (mouse_clicked->button == sf::Mouse::Button::Middle)
                    on_mouse_press(InputButton::MIDDLE);
            }
            else if (const auto* mouse_released = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (mouse_released->button == sf::Mouse::Button::Left)
                    on_mouse_release(InputButton::LEFT);
                else if (mouse_released->button == sf::Mouse::Button::Right)
                    on_mouse_release(InputButton::RIGHT);
                else if (mouse_released->button == sf::Mouse::Button::Middle)
                    on_mouse_release(InputButton::MIDDLE);
            }
            else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>())
            {
                Input().add_key_press(key_pressed->code);
            }
            else if (const auto* text_entered = event->getIf<sf::Event::TextEntered>())
            {
                keyboard_string += text_entered->unicode;
            }
        }
        
        // Store delta time, which can later be queried by any module

        delta_time = clock.restart().asSeconds();

        // CTRL+P: Toggle mini terminal
        // Escape tries to clear text, and exits if nothing was there
        // Enter works a similar way, but submits the command
        // Command parsing is handled by this class and the CommandBar is
        // instructed on what to do next based on the result

        if (using_terminal && Input().check_key_press(SF_KEY::Escape))
        {
            using_terminal = command_bar->attempt_clear();
            if (!using_terminal)
                command_bar->set_typing(false);
        }
        if (using_terminal && Input().check_key_press(SF_KEY::Enter, KeyMod::NONE, KeyMod::ALL))
        {
            CommandResult command = command_parser.parse(command_bar->get_command());
            if (command_bar->get_command().empty())
            {
                using_terminal = false;
                command_bar->set_typing(false);
            }
            else if (!command.valid())
            {
                command_bar->submit_error();
                log_module->push_error(command.get_error());
            }
            else
            {
                try
                {
                    log_module->push_command(execute_command(command));
                    command_bar->submit_valid();
                }
                catch (ExecuteException error)
                {
                    command_bar->submit_error();
                    log_module->push_error(error.message());
                }
            }
        }
        if (Input().check_key_press(SF_KEY::P, KeyMod::CTRL))
        {
            using_terminal = !using_terminal;
            command_bar->set_typing(using_terminal);
        }
        if (using_terminal)
        {
            command_bar->update(keyboard_string);
        }
        else
        {
            for (EditorModule** module : visible_modules)
            {
                (*module)->update();
            }
        }

        // Ctrl + (technically =) or -: Modify window scale
        // For now: limited between 70% and 200%

        int old_scale_index = ui_scale_index;
        if (Input().check_key_press(SF_KEY::Equal, KeyMod::CTRL))
            ui_scale_index = std::min(ui_scale_index + 1, 10);
        if (Input().check_key_press(SF_KEY::Hyphen, KeyMod::CTRL))
            ui_scale_index = std::max(ui_scale_index - 1, -3);
        if (old_scale_index != ui_scale_index)
        {
            ui_scale = 1.f + (float)ui_scale_index * 0.1f;
            resize_modules();
            UserSettings user_settings = FileManager().get_user_settings();
            user_settings.ui_scale_index = ui_scale_index;
            FileManager().update_user_settings(user_settings);
        }

        if (!using_terminal)
        {
            // Alt+Q/W: Next/prev tab (wraps!)
            
            if (!flex_tabs.empty() && Input().check_key_press(SF_KEY::Q, KeyMod::ALT))
                switch_flex_tab(mod((int) current_flex_tab - 1, (int) flex_tabs.size()));
            if (!flex_tabs.empty() && Input().check_key_press(SF_KEY::W, KeyMod::ALT))
                switch_flex_tab(mod((int) current_flex_tab + 1, (int) flex_tabs.size()));

            // Alt+Arrow: Move timeline

            if (Input().check_key_press(SF_KEY::Left, KeyMod::ALT))
                timeline_module->scroll_left();
            if (Input().check_key_press(SF_KEY::Right, KeyMod::ALT))
                timeline_module->scroll_right();
            if (Input().check_key_press(SF_KEY::Down, KeyMod::ALT))
                timeline_module->zoom_out();
            if (Input().check_key_press(SF_KEY::Up, KeyMod::ALT))
                timeline_module->zoom_in();

            // 1,2: Move playhead
            // Ctrl = 10 frames

            if (Input().check_key_press(SF_KEY::Num1))
                timeline_module->playhead_backward(1);
            if (Input().check_key_press(SF_KEY::Num2))
                timeline_module->playhead_forward(1);
            if (Input().check_key_press(SF_KEY::Num1, KeyMod::CTRL))
                timeline_module->playhead_backward(10);
            if (Input().check_key_press(SF_KEY::Num2, KeyMod::CTRL))
                timeline_module->playhead_forward(10);

            // Alt+Home,End: Move playhead to start/end
            // F: Focus playhead
            // TimelineModule has a separate check for these that doesn't
            // require ALT if the module is focused

            if (Input().check_key_press(SF_KEY::Home, KeyMod::ALT))
                timeline_module->playhead_to_start();
            if (Input().check_key_press(SF_KEY::End, KeyMod::ALT))
                timeline_module->playhead_to_end();
            if (Input().check_key_press(SF_KEY::F, KeyMod::ALT))
                timeline_module->focus_playhead();

            // Alt+1,2,3: Focus module
            // A little unintuitive that the timeline module gets hotkey 1,
            // but it is by far the most often focused and deserves the priority
            // Alt+4/D: Unfocus modules

            if (Input().check_key_press(SF_KEY::Num1, KeyMod::ALT))
                focus_module(timeline_module);
            if (Input().check_key_press(SF_KEY::Num2, KeyMod::ALT))
                focus_module(preview_module);
            if (Input().check_key_press(SF_KEY::Num3, KeyMod::ALT))
                focus_module(flex_module);
            if (Input().check_key_press(SF_KEY::Num4, KeyMod::ALT) || Input().check_key_press(SF_KEY::D, KeyMod::ALT))
                focus_module(nullptr);
        }

        // Update all visible modules

        if (!using_terminal)
        {
            for (EditorModule** module : visible_modules)
            {
                (*module)->on_update();
            }
        }

        // Update export display (if applicable)

        if (exporting)
        {
            if (locked_project->is_exporting())
            {
                std::string export_string = "Exporting: " + std::to_string(locked_project->get_export_percentage()) + "%";
                command_bar->set_status_exporting(export_string);
            }
            else
            {
                exporting = false;
                command_bar->set_status_exporting("");
                unlock_project();
            }
        }

        // Load a new project if needed
        // This is buffered until the end of the update loop to prevent
        // weirdness from switching out project data mid-function

        if (queued_project != nullptr)
            load_project(queued_project);

        // Update debug module with most recent info

        std::stringstream debug_info {};
        debug_info << "Cursor: (" << mouse_position.x << ", " << mouse_position.y << ")";
        debug_module->refresh_info(debug_info.str());

        // Display the root, which will propogate to all other GLNode children

        Graphics().display(root.get());
    }
}

float Editor::get_delta_time()
{
    return delta_time;
}

void Editor::set_cursor(sf::Cursor::Type cursor_type)
{
    window->setMouseCursor(sf::Cursor{cursor_type});
}

bool Editor::set_drag_event(std::unique_ptr<DragMouseEvent> new_event)
{
    if (drag_mouse_event != nullptr)
        return false;
    drag_mouse_event = std::move(new_event);
    drag_mouse_event->source_pos = mouse_position;
    drag_mouse_event->current_pos = mouse_position;
    drag_mouse_event->create_node(root.get());
    drag_mouse_event->update_node(sf::Vector2f(mouse_position));
    return true;
}

DragMouseEvent* Editor::get_drag_event()
{
    return drag_mouse_event.get();
}

void Editor::cancel_drag_event()
{
    // Normal callbacks are not run when cancelled
    
    if (drag_mouse_event != nullptr)
    {
        drag_mouse_event->delete_node();
        drag_mouse_event.release();
    }
}

CommandParser& Editor::get_command_parser()
{
    return command_parser;
}

std::string Editor::run_command(std::string command, bool throw_errors)
{
    CommandResult result = command_parser.parse(command_bar->get_command());
    if (!result.valid())
        return "";
    try
    {
        return execute_command(result);
    }
    catch (ExecuteException error)
    {
        if (throw_errors)
            throw error;
        return "";
    }
}

Project* Editor::get_project()
{
    return project;
}

LockedProject* Editor::get_locked_project()
{
    return locked_project;
}

// The arguments are stored as a sequence of void*
// Although a little unsafe, this is still fine in theory since all notification types
// should have a well-defined sequence of argument types

void Editor::notify_modules(int notif_class, int notif_type, size_t num_args, void** arg_ptrs)
{
    // Even non-visible modules are notified

    for (EditorModule* module : all_modules)
    {
        if (module->receives_notifs(notif_class))
            module->on_notif(notif_class, notif_type, num_args, arg_ptrs);
    }

    // The Editor can also react to notifications

    if (notif_class == NOTIF_PROJECT_INFO::ID)
    {
        if (notif_type == NOTIF_PROJECT_INFO::LENGTH_CHANGED)
        {
            command_bar->set_status_length(project->get_project_length_approx());
        }
    }
}

void Editor::load_project(Project* new_project)
{
    project = new_project;
    locked_project = (LockedProject*) project;
    exporting = false;

    // All child nodes are first be orphaned
    // This by itself doesn't free the memory
    // The memory is instead freed when the respective smart pointers are
    // freed during module deletion and similar events

    root->free_children();

    // Make sure any leftover data is reset first

    delete preview_module;
    delete timeline_module;
    delete log_module;
    delete media_module;
    delete project_module;
    delete debug_module;
    delete command_bar;

    using_terminal = false;
    all_modules.clear();
    visible_modules.clear();
    flex_tabs.clear();
    flex_module = nullptr;
    focused_module = nullptr;

    // Module setup

    preview_module = new EditorModule(*this);
    timeline_module = new TimelineModule(*this);
    command_bar = new CommandBar(*this);
    log_module = new LogModule(*this);
    media_module = new MediaModule(*this);
    project_module = new ProjectModule(*this);
    debug_module = new DebugModule(*this);
    visible_modules.insert(visible_modules.end(), {&preview_module, &flex_module, (EditorModule**) &timeline_module});
    all_modules.insert(all_modules.end(), {
        preview_module,
        timeline_module,
        log_module,
        media_module,
        project_module,
        debug_module
    });

    // Flex module setup
    // Like the modules themselves, tab parameters are set during `resize_modules()`

    current_flex_tab = 0;
    flex_tabs.push_back(std::unique_ptr<FlexTab>(new FlexTab(*this, log_module, "Log")));
    flex_tabs.push_back(std::unique_ptr<FlexTab>(new FlexTab(*this, media_module, "Media")));
    flex_tabs.push_back(std::unique_ptr<FlexTab>(new FlexTab(*this, project_module, "Project")));
    flex_tabs.push_back(std::unique_ptr<FlexTab>(new FlexTab(*this, debug_module, "Debug")));
    flex_tabs.at(current_flex_tab)->set_selected(true);
    flex_module = &flex_tabs.at(current_flex_tab)->get_module();

    // Node tree setup
    // Nodes are drawn in the order they're added to the vector

    root->add_child(preview_module->get_node());
    root->add_child(timeline_module->get_node());
    root->add_child(command_bar->get_node());
    for (std::unique_ptr<FlexTab>& tab : flex_tabs)
        root->add_child(tab->get_node());
    for (std::unique_ptr<FlexTab>& tab : flex_tabs)
        root->add_child(tab->get_module().get_node());
    temp_menu_bar.reset(GLContainer::create(root.get(), sf::Vector2f(), sf::Vector2f()));
    menu_bar_text.reset(GLText::create(temp_menu_bar.get(), Graphics().main_font(), 0u, "File   Edit   Settings   Export"));

    // Other project setup

    project_module->refresh_info();
    command_bar->set_status_name(project->get_name());
    command_bar->set_status_length(project->get_project_length_approx());

    // Reset mouse events

    drag_mouse_event.release();

    // Now that nodes are setup, properly set their sizes
    
    resize_modules();

    queued_project = nullptr;
}

void Editor::on_resized(sf::Vector2i new_size)
{
    Graphics().on_window_resized(root.get());
    x_divider = (int)( ((float)x_divider / window_size.x) * new_size.x);
    y_divider = (int)( ((float)y_divider / window_size.y) * new_size.y);
    x_divider = std::max(MODULE_MARGIN, std::min(new_size.x - MODULE_MARGIN, x_divider));
    y_divider = std::max(MODULE_MARGIN, std::min(new_size.y - MODULE_MARGIN, y_divider));
    window_size = new_size;
    resize_modules();
}

void Editor::on_mouse_move(sf::Vector2i position)
{
    mouse_position = position;

    // Update mouse drag event, if applicable

    if (drag_mouse_event != nullptr)
    {
        drag_mouse_event->current_pos = mouse_position;
        drag_mouse_event->on_move();
        drag_mouse_event->update_node(sf::Vector2f(mouse_position));
    }

    // Special behavior for dragging divider between modules
    
    DragDivider* drag_divider_event = nullptr;
    if (drag_mouse_event != nullptr)
        drag_divider_event = dynamic_cast<DragDivider*>(drag_mouse_event.get());

    // Highlight modules & tabs (but not if moving dividers)
    // Also run module-defined update callbacks

    if (drag_divider_event == nullptr)
    {
        for (EditorModule** module : visible_modules)
        {
            sf::IntRect module_bounds = (*module)->get_bounds();
            bool mouse_overlaps = module_bounds.contains(mouse_position);
            (*module)->set_hover_highlight(mouse_overlaps);
            DragMouseEvent* event_ptr = (drag_mouse_event && drag_mouse_event->target == *module) ? drag_mouse_event.get() : nullptr;
            (*module)->on_mouse_move(mouse_position - module_bounds.position, mouse_overlaps, event_ptr);
        }
        for (std::unique_ptr<FlexTab>& tab : flex_tabs)
        {
            bool mouse_overlaps = tab->get_bounds().contains(mouse_position);
            tab->set_hovering(mouse_overlaps);
        }
    }

    // Scale sub-windows if dividers were moved

    if (drag_mouse_event != nullptr)
    {
        if (drag_divider_event != nullptr)
        {
            if (drag_divider_event->is_vertical() && mouse_position.y > MODULE_MARGIN && mouse_position.y < (window_size.y - MODULE_MARGIN))
            {
                y_divider = mouse_position.y;
                resize_modules();
            }
            if (!drag_divider_event->is_vertical() && mouse_position.x > MODULE_MARGIN && mouse_position.x < (window_size.x - MODULE_MARGIN))
            {
                x_divider = mouse_position.x;
                resize_modules();
            }
        }
    }
    else
    {
        // Set appropriate cursor type when not dragging
        // Only reset the cursor when explicitly moving off of the divider
        // This prevents it from constantly being reset and overridding
        // other modules' behavior

        if (abs(mouse_position.y - y_divider) < 6)
        {
            set_cursor(sf::Cursor::Type::SizeVertical);
            hovering_divider = true;
        }
        else if (mouse_position.y < y_divider && abs(mouse_position.x - x_divider) < 6)
        {
            set_cursor(sf::Cursor::Type::SizeHorizontal);
            hovering_divider = true;
        }
        else if (hovering_divider)
        {
            set_cursor(sf::Cursor::Type::Arrow);
            hovering_divider = false;
        }
    }
}

void Editor::on_mouse_press(InputButton button)
{
    // Start scaling dividers if they were clicked

    if (abs(mouse_position.y - y_divider) < 6)
        set_drag_event(std::unique_ptr<DragMouseEvent>(new DragDivider(true)));
    else if (mouse_position.y < y_divider && abs(mouse_position.x - x_divider) < 6)
        set_drag_event(std::unique_ptr<DragMouseEvent>(new DragDivider(false)));

    // Switch tabs if clicked
    // In the future, it might be desirable to have this occur
    // on mouse release instead

    for (int i = 0; i < flex_tabs.size(); ++i)
    {
        if (flex_tabs.at(i)->get_bounds().contains(mouse_position))
            switch_flex_tab(i);
    }

    // Switch focus if mouse is inside a module
    // Sending `nullptr` to focus_module unfocuses everything

    EditorModule* new_focus = nullptr;
    for (EditorModule** module : visible_modules)
    {
        if ((*module)->get_bounds().contains(mouse_position))
            new_focus = *module;
    }
    focus_module(new_focus);
    
    // Trigger callback for each module

    for (EditorModule** module : visible_modules)
    {
        sf::IntRect module_bounds = (*module)->get_bounds();
        bool mouse_overlaps = module_bounds.contains(mouse_position);
        (*module)->on_mouse_press(mouse_position - module_bounds.position, mouse_overlaps, button);
    }
}

void Editor::on_mouse_release(InputButton button)
{
    if (drag_mouse_event != nullptr)
    {
        drag_mouse_event->delete_node();
        drag_mouse_event->on_release();
    }

    // Trigger callback for each module
    // If the cursor is within it, trigger the event drop callback

    for (EditorModule** module : visible_modules)
    {
        sf::IntRect module_bounds = (*module)->get_bounds();
        bool mouse_overlaps = module_bounds.contains(mouse_position);
        DragMouseEvent* event_ptr = (drag_mouse_event && drag_mouse_event->target == *module) ? drag_mouse_event.get() : nullptr;
        (*module)->on_mouse_release(mouse_position - module_bounds.position, mouse_overlaps, button, event_ptr);
        if (drag_mouse_event != nullptr && mouse_overlaps)
            (*module)->on_mouse_drop(mouse_position - module_bounds.position, drag_mouse_event.get());
    }

    // A little confusing, but the call to release() is freeing the pointer,
    // not simulating a mouse release :)
    
    drag_mouse_event.release();
}

sf::Vector2i Editor::get_mouse_position()
{
    return mouse_position;
}

// Called on window resize, bounds movement, or UI scale change

void Editor::resize_modules()
{
    int tab_height = (int)(TAB_HEIGHT * ui_scale);
    int cmd_height = (int)(C_BAR_HEIGHT * ui_scale);
    int tab_width = (int)(TAB_WIDTH * ui_scale);

    // Modules

    preview_module->set_bounds(sf::IntRect({0, tab_height}, {x_divider, y_divider - tab_height}));
    preview_module->set_ui_scale(ui_scale);
    timeline_module->set_bounds(sf::IntRect({0, y_divider}, {window_size.x, window_size.y - y_divider - cmd_height}));
    timeline_module->set_ui_scale(ui_scale);
    command_bar->set_bounds(sf::IntRect({0, window_size.y - cmd_height}, {window_size.x, cmd_height}));
    command_bar->set_ui_scale(ui_scale);

    // Tabs

    for (int i = 0; i < flex_tabs.size(); ++i)
    {
        FlexTab& flex_tab = *flex_tabs.at(i);
        flex_tab.set_bounds(sf::IntRect({x_divider + 8 + (tab_width + 5) * i, 8}, {tab_width, tab_height - 3}));
        flex_tab.set_ui_scale(ui_scale);
        flex_tab.get_module().set_bounds(sf::IntRect({x_divider, tab_height}, {window_size.x - x_divider, y_divider - tab_height}));
        flex_tab.get_module().set_ui_scale(ui_scale);
    }

    // Misc graphics

    temp_menu_bar->set_size(sf::Vector2f(x_divider, tab_height));
    menu_bar_text->set_position(sf::Vector2f(sf::Vector2i(10 + 4 * ui_scale, 4 + 4 * ui_scale)));
    menu_bar_text->set_char_size((unsigned int)(19.f * ui_scale));
}

void Editor::focus_module(EditorModule* module)
{
    if (module != focused_module)
    {
        if (focused_module != nullptr)
            focused_module->set_focused(false);
        focused_module = module;
        if (focused_module != nullptr)
            focused_module->set_focused(true);
    }
}

void Editor::switch_flex_tab(unsigned int index)
{
    flex_tabs.at(current_flex_tab)->set_selected(false);
    flex_tabs.at(index)->set_selected(true);
    current_flex_tab = index;
    flex_module = &flex_tabs.at(index)->get_module();
}

void Editor::lock_project()
{
    project = nullptr;
}

void Editor::unlock_project()
{
    Project* downcast = dynamic_cast<Project*>(locked_project);
    if (downcast == nullptr)
        throw ButterException("Attempted to unlock invalid project");
    project = downcast;
}
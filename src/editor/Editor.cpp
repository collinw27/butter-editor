#include "editor/Editor.h"

#include "editor/core/DragDivider.h"
#include "editor/core/TestModule.h"

#include "utility/Graphics.h"
#include "utility/Input.h"
#include "utility/Logger.h"
#include "utility/FileManager.h"
#include "utility/UserSettings.h"

constexpr int MODULE_MARGIN = 100;
constexpr int TAB_HEIGHT = 30;
constexpr int TAB_WIDTH = 90;
constexpr int CMD_HEIGHT = 20;

const sf::Color Editor::C_BG{0, 0, 0};
const sf::Color Editor::C_FG{90, 90, 90};
const sf::Color Editor::C_HOVER{130, 130, 130};
const sf::Color Editor::C_FG_DESELECTED{50, 50, 50};
const sf::Color Editor::C_HIGHLIGHT{90, 90, 90};

Editor::Editor()
{
    // SFML setup
    
    Graphics().init(sf::VideoMode({1280, 720}), "Butter Video Editor", sf::Style::Close | sf::Style::Resize | sf::Style::Titlebar);
    Graphics().set_clear_color(Editor::C_BG);
    window = &Graphics().get_window();
    window_size = sf::Vector2i(window->getSize());
    window->setMinimumSize(sf::Vector2u(300, 200));
    window->setFramerateLimit(150);
    root = GLRootNode::create();

    // Module setup (flex initialized later)

    preview_module = new EditorModule(*this);
    timeline_module = new EditorModule(*this);
    command_bar = new CommandBar(*this);
    visible_modules.insert(visible_modules.end(), {&preview_module, &flex_module, &timeline_module});

    // Flex module setup
    // Like the modules themselves, tab parameters are set during `resize_modules()`

    current_flex_tab = 0;
    flex_tabs.push_back(new FlexTab(*this, new TestModule(*this, "Test module #1"), "Test 1"));
    flex_tabs.push_back(new FlexTab(*this, new TestModule(*this, "Test module #2"), "Test 2"));
    flex_tabs.at(current_flex_tab)->set_selected(true);
    flex_module = &flex_tabs.at(current_flex_tab)->get_module();

    // Node tree setup
    // Currently, nodes are drawn in the order they're added to the vector
    // There will likely be support for layers added later on

    // temp_menu_bar = new sf::Text(FileManager().get_font(), "File   Edit   Settings   Export");
    root->add_child(preview_module->get_node());
    root->add_child(timeline_module->get_node());
    root->add_child(command_bar->get_node());
    for (FlexTab* tab : flex_tabs)
        root->add_child(tab->get_node());
    for (FlexTab* tab : flex_tabs)
        root->add_child(tab->get_module().get_node());
    temp_menu_bar = GLText::create(root, sf::Vector2f(), "File   Edit   Settings   Export");

    // UI parameters
    // `resize_modules()` must ALWAYS be called
    // Module properties are dependent on calculations whose implementation
    // are only provided in said function
    
    y_divider = 360;
    x_divider = 640;
    ui_scale_index = FileManager().get_user_settings().ui_scale_index;
    ui_scale = 1.f + (float)ui_scale_index * 0.1f;
    resize_modules();

    clock = sf::Clock();
    clock.start();
}

Editor::~Editor()
{
    delete window;
    delete preview_module;
    delete timeline_module;
    for (auto flex_tab : flex_tabs)
    {
        delete flex_tab;
    }
    delete temp_menu_bar;
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
                on_mouse_moved(mouse_moved->position);
            }
            else if (const auto* mouse_clicked = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse_clicked->button == sf::Mouse::Button::Left)
                {
                    on_mouse_pressed();
                }
            }
            else if (const auto* mouse_clicked = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (mouse_clicked->button == sf::Mouse::Button::Left)
                {
                    on_mouse_released();
                }
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

        if (using_terminal && Input().check_key_press(sf::Keyboard::Key::Escape))
        {
            using_terminal = command_bar->attempt_clear();
            if (!using_terminal)
                command_bar->set_typing(false);
        }
        if (Input().check_key_press(sf::Keyboard::Key::P, KeyMod::CTRL))
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

        Graphics().display(root);
    }
}

void Editor::set_cursor(sf::Cursor::Type cursor_type)
{
    window->setMouseCursor(sf::Cursor{cursor_type});
}

float Editor::get_delta_time()
{
    return delta_time;
}

// void Editor::draw(sf::RenderWindow& window)
// {
//     // Draw tabs at top

//     for (auto flex_tab : flex_tabs)
//     {
//         flex_tab->draw(window);
//     }

//     // Ignore non-selected flex modules

//     for (EditorModule** module : visible_modules)
//     {
//         (*module)->draw(window);
//     }
//     window.draw(*temp_menu_bar);
//     command_bar->draw(window);
// }

void Editor::on_resized(sf::Vector2i new_size)
{
    Graphics().on_window_resized(root);
    x_divider = (int)( ((float)x_divider / window_size.x) * new_size.x);
    y_divider = (int)( ((float)y_divider / window_size.y) * new_size.y);
    x_divider = std::max(MODULE_MARGIN, std::min(new_size.x - MODULE_MARGIN, x_divider));
    y_divider = std::max(MODULE_MARGIN, std::min(new_size.y - MODULE_MARGIN, y_divider));
    // window->setView(sf::View(sf::FloatRect(sf::Vector2f(), sf::Vector2f(new_size))));
    window_size = new_size;
    resize_modules();
}

void Editor::on_mouse_moved(sf::Vector2i position)
{
    mouse_position = position;
    
    DragDivider* drag_divider_event = nullptr;
    if (drag_mouse_event != nullptr)
        drag_divider_event = dynamic_cast<DragDivider*>(drag_mouse_event);

    // Highlight modules & tabs (but not if moving dividers)

    if (drag_divider_event == nullptr)
    {
        for (EditorModule** module : visible_modules)
        {
            bool mouse_overlaps = (*module)->get_bounds().contains(mouse_position);
            (*module)->set_hover_highlight(mouse_overlaps);
        }
        for (FlexTab* tab : flex_tabs)
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

        if (abs(mouse_position.y - y_divider) < 6)
            set_cursor(sf::Cursor::Type::SizeVertical);
        else if (mouse_position.y < y_divider && abs(mouse_position.x - x_divider) < 6)
            set_cursor(sf::Cursor::Type::SizeHorizontal);
        else
            set_cursor(sf::Cursor::Type::Arrow);
    }
}

void Editor::on_mouse_pressed()
{
    // Start scaling dividers if they were clicked

    if (abs(mouse_position.y - y_divider) < 6)
        drag_mouse_event = new DragDivider(mouse_position, true);
    else if (mouse_position.y < y_divider && abs(mouse_position.x - x_divider) < 6)
        drag_mouse_event = new DragDivider(mouse_position, false);

    // Switch tabs if clicked
    // In the future, it might be desirable to have this occur
    // on mouse release instead

    for (int i = 0; i < flex_tabs.size(); ++i)
    {
        FlexTab* tab = flex_tabs.at(i);
        if (tab->get_bounds().contains(mouse_position))
        {
            flex_tabs.at(current_flex_tab)->set_selected(false);
            tab->set_selected(true);
            current_flex_tab = i;
            flex_module = &tab->get_module();
        }
    }
}

void Editor::on_mouse_released()
{
    delete drag_mouse_event;
    drag_mouse_event = nullptr;
}

sf::Vector2i Editor::get_mouse_position()
{
    return mouse_position;
}

// Called on window resize, bounds movement, or UI scale change

void Editor::resize_modules()
{
    int tab_height = (int)(TAB_HEIGHT * ui_scale);
    int cmd_height = (int)(CMD_HEIGHT * ui_scale);
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

    temp_menu_bar->set_position(sf::Vector2f(sf::Vector2i(10 + 4 * ui_scale, 8 + 2 * ui_scale)));
    temp_menu_bar->set_char_size((unsigned)(19.f * ui_scale));
}
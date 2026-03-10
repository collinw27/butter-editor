#include "editor/Editor.h"

#include "editor/DragDivider.h"
#include "utility/Input.h"
#include "utility/Logger.h"

constexpr int MODULE_MARGIN = 100;

Editor::Editor()
{
    window = new sf::RenderWindow(sf::VideoMode({1280, 720}), "Butter Video Editor", sf::Style::Close | sf::Style::Resize | sf::Style::Titlebar);
    window_size = sf::Vector2i(window->getSize());

    y_divider = 360;
    x_divider = 640;
    preview_module = new EditorModule(*this, sf::IntRect({0, 0}, {x_divider, y_divider}));
    config_module = new EditorModule(*this, sf::IntRect({x_divider, 0}, {window_size.x - x_divider, y_divider}));
    timeline_module = new EditorModule(*this, sf::IntRect({0, y_divider}, {window_size.x, window_size.y - y_divider}));

    modules.insert(modules.end(), {preview_module, config_module, timeline_module});
}

Editor::~Editor()
{
    delete window;
    delete preview_module;
    delete config_module;
    delete timeline_module;
}

void Editor::run()
{
    while (window->isOpen())
    {
        Input::singleton()->clear_keys();

        while (const std::optional event = window->pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window->close();
            }
            else if (const auto *resized = event->getIf<sf::Event::Resized>())
            {
                on_resized(sf::Vector2i(resized->size));
            }
            else if (const auto *mouse_moved = event->getIf<sf::Event::MouseMoved>())
            {
                on_mouse_moved(mouse_moved->position);
            }
            else if (const auto *mouse_clicked = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse_clicked->button == sf::Mouse::Button::Left)
                {
                    on_mouse_pressed();
                }
            }
            else if (const auto *mouse_clicked = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (mouse_clicked->button == sf::Mouse::Button::Left)
                {
                    on_mouse_released();
                }
            }
            else if (const auto *key_pressed = event->getIf<sf::Event::KeyPressed>())
            {
                Input::singleton()->add_key_press(key_pressed->code);
            }
        }

        for (auto module : modules)
        {
            module->update();
        }

        window->clear();
        draw(*window);
        window->display();
    }
}

void Editor::set_cursor(sf::Cursor::Type cursor_type)
{
    window->setMouseCursor(sf::Cursor{cursor_type});
}

void Editor::draw(sf::RenderWindow& window)
{
    for (auto module : modules)
    {
        module->draw(window);
    }
}

void Editor::on_resized(sf::Vector2i new_size)
{
    x_divider = (int)( ((float)x_divider / window_size.x) * new_size.x);
    y_divider = (int)( ((float)y_divider / window_size.y) * new_size.y);
    x_divider = std::max(MODULE_MARGIN, std::min(new_size.x - MODULE_MARGIN, x_divider));
    y_divider = std::max(MODULE_MARGIN, std::min(new_size.y - MODULE_MARGIN, y_divider));
    window->setView(sf::View(sf::FloatRect(sf::Vector2f(), sf::Vector2f(new_size))));
    window_size = new_size;
    resize_modules();
}

void Editor::on_mouse_moved(sf::Vector2i position)
{
    mouse_position = position;
    for (auto module : modules)
    {
        bool mouse_overlaps = module->get_bounds().contains(mouse_position);
        module->set_hover_highlight(mouse_overlaps);
    }

    // Scale sub-windows if dividers were moved

    if (drag_mouse_event != nullptr)
    {
        DragDivider *drag_divider_event = dynamic_cast<DragDivider*>(drag_mouse_event);
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

    // Set appropriate cursor type when not dragging

    else
    {
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

void Editor::resize_modules()
{
    preview_module->set_bounds(sf::IntRect({0, 0}, {x_divider, y_divider}));
    config_module->set_bounds(sf::IntRect({x_divider, 0}, {window_size.x - x_divider, y_divider}));
    timeline_module->set_bounds(sf::IntRect({0, y_divider}, {window_size.x, window_size.y - y_divider}));
}
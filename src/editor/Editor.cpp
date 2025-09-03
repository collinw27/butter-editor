#include "editor/Editor.h"

#include "utility/Input.h"

Editor::Editor()
{
    window = new sf::RenderWindow(sf::VideoMode({1280, 720}), "Butter Video Editor", sf::Style::Close);

    preview_module = new EditorModule(*this, sf::IntRect({0, 0}, {640, 360}));
    config_module = new ConfigModule(*this);
    timeline_module = new TimelineModule(*this);

    modules.insert(modules.end(), {preview_module, config_module, timeline_module});
}

Editor::~Editor()
{
    delete window;
    delete preview_module;
    delete config_module;
    delete timeline_module;
    delete drag_mouse_event;
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

void Editor::draw(sf::RenderWindow& window)
{
    for (auto module : modules)
    {
        module->draw(window);
    }
    if (drag_mouse_event)
    {
        drag_mouse_event->draw(window);
    }
}

void Editor::on_mouse_moved(sf::Vector2i position)
{
    mouse_position = position;
    for (auto module : modules)
    {
        bool mouse_overlaps = module->get_rect().contains(mouse_position);
        module->set_hover_highlight(mouse_overlaps);
        module->set_drag_highlight(drag_mouse_event != nullptr);
        module->on_mouse_moved(mouse_position, mouse_overlaps);
    }
    if (drag_mouse_event)
    {
        drag_mouse_event->on_move(mouse_position);
    }
}

void Editor::on_mouse_pressed()
{
    for (auto module : modules)
    {
        module->on_mouse_pressed(mouse_position, module->get_rect().contains(mouse_position));
    }
}

void Editor::on_mouse_released()
{
    for (auto module : modules)
    {
        module->set_drag_highlight(false);
        module->on_mouse_released(mouse_position, module->get_rect().contains(mouse_position), drag_mouse_event);
    }
    delete drag_mouse_event;
    drag_mouse_event = nullptr;
}

sf::Vector2i Editor::get_mouse_position()
{
    return mouse_position;
}

void Editor::new_mouse_event(DragMouse *new_event)
{
    delete drag_mouse_event;
    drag_mouse_event = new_event;
}
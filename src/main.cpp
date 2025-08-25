#include <SFML/Graphics.hpp>
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/avutil.h>
    #include <libswscale/swscale.h>
}

#include "editor/Editor.h"
#include "utility/ResourceManager.h"
#include "utility/Logger.h"

int main()
{
    ResourceManager *resource_manager = new ResourceManager();
    Logger *logger = new Logger();

    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Butter Video Editor", sf::Style::Close);

    Editor* editor = new Editor();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto *mouse_moved = event->getIf<sf::Event::MouseMoved>())
            {
                editor->on_mouse_moved(mouse_moved->position);
            }
            else if (const auto *mouse_clicked = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse_clicked->button == sf::Mouse::Button::Left)
                {
                    editor->on_mouse_pressed();
                }
            }
            else if (const auto *mouse_clicked = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (mouse_clicked->button == sf::Mouse::Button::Left)
                {
                    editor->on_mouse_released();
                }
            }
        }

        editor->update();
        window.clear();
        editor->draw(window);
        window.display();
    }
}
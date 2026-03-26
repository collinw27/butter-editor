#include <utility>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <gl/glew.h>
#include <SFML/OpenGL.hpp>

#include "utility/FileManager.h"
#include "utility/Graphics.h"
#include "utility/Exceptions.h"
#include "graphics/glRectangle.h"

int main()
{
    try{
    FileManagerSingleton* file_manager = new FileManagerSingleton();
    GraphicsSingleton* graphics = new GraphicsSingleton();

    Graphics().create_window(sf::VideoMode({1280, 720}), "GLTest", sf::Style::Default);
    sf::RenderWindow& window = Graphics().get_window();
    window.setVerticalSyncEnabled(true);
        
    if (glewInit() != GLEW_OK)
        throw ButterException("Cannot initialize GLEW");

    if (!window.setActive(true))
        throw ButterException("Error requesting OpenGL context");
    GLRectangle rect {sf::Vector2f(10, 10), sf::Vector2f(2000, 100)};
    rect.set_fill_color(sf::Color::Red);
    std::ignore = window.setActive(false);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                rect.on_window_resized();
            }
        }
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, window.getSize().x, window.getSize().y);

        std::ignore = window.setActive(true);
        rect.draw();
        glBindVertexArray(0);
        glUseProgram(0);
        std::ignore = window.setActive(false);
        window.display();
    }
    }
    catch (const ButterException& e)
    {
        e.print();
    }
}
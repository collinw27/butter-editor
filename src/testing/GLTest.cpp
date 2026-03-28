#include <utility>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <gl/glew.h>
#include <SFML/OpenGL.hpp>

#include "utility/FileManager.h"
#include "utility/Graphics.h"
#include "utility/Exceptions.h"
#include "graphics/GLRootNode.h"
#include "graphics/GLContainer.h"
#include "graphics/GLRectangle.h"
#include "graphics/GLText.h"

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
    GLRootNode* root = GLRootNode::create();
    GLContainer* container = GLContainer::create(root, sf::Vector2f(0, 0), sf::Vector2f(1000, 1000));
    GLRectangle* rect = GLRectangle::create(container, sf::Vector2f(10, 10), sf::Vector2f(500, 100));
    GLText* text = GLText::create(container, sf::Vector2f(10, 200), "asdf");
    text->set_char_size(20);
    rect->set_fill_color(sf::Color::Red);
    std::ignore = window.setActive(false);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                Graphics().on_window_resized();
                root->on_window_resized_all();
            }
        }
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, window.getSize().x, window.getSize().y);

        std::ignore = window.setActive(true);
        root->draw_all();
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
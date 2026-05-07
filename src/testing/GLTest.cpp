#include <utility>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <gl/glew.h>
#include <SFML/OpenGL.hpp>

#include "utility/FileManager.h"
#include "utility/Graphics.h"
#include "utility/core.h"
#include "graphics/nodes.h"

int main()
{
    try {

    FileManagerSingleton* file_manager = new FileManagerSingleton();
    GraphicsSingleton* graphics = new GraphicsSingleton();

    Graphics().init(sf::VideoMode({1280, 720}), "GLTest", sf::Style::Default);

    GLRootNode* root = GLRootNode::create();

    GLText* text_1 = GLText::create(root, Graphics().main_font(), 10u, "The quick brown fox");
    text_1->enable_special_formatting();
    text_1->add_color(sf::Color::Red, 1);
    text_1->add_color(sf::Color::Green);
    text_1->add_string(" jumps over the lazy dog.");
    text_1->set_position(sf::Vector2f(10, 10));
    text_1->set_char_size(40u);
    text_1->set_color(sf::Color::Blue);

    sf::RenderWindow& window = Graphics().get_window();
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
                Graphics().on_window_resized(root);
        }
        
        Graphics().display(root);
    }
    }
    catch (const ButterException& e)
    {
        e.print();
    }
}
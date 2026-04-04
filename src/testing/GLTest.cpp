#include <utility>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <gl/glew.h>
#include <SFML/OpenGL.hpp>

#include "utility/FileManager.h"
#include "utility/Graphics.h"
#include "utility/Exceptions.h"
#include "graphics/nodes.h"

int main()
{
    try{

    FileManagerSingleton* file_manager = new FileManagerSingleton();
    GraphicsSingleton* graphics = new GraphicsSingleton();

    Graphics().init(sf::VideoMode({1280, 720}), "GLTest", sf::Style::Default);

    GLRootNode* root = GLRootNode::create();
    GLNode* displacement = GLNode::create(root);
    displacement->set_position(sf::Vector2f(0, 50));
    GLContainer* container = GLContainer::create(displacement, sf::Vector2f(0, 30), sf::Vector2f(100, 100));

    GLText* text_1 = GLText::create(container, Graphics().main_font(), 10u, "The quick brown fox jumps over the lazy dog.");
    text_1->set_position(sf::Vector2f(0, -30));
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
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
#include "graphics/GLOutlinedRectangle.h"
#include "graphics/GLText.h"

int main()
{
    try{

    FileManagerSingleton* file_manager = new FileManagerSingleton();
    GraphicsSingleton* graphics = new GraphicsSingleton();

    Graphics().init(sf::VideoMode({1280, 720}), "GLTest", sf::Style::Default);

    GLRootNode* root = GLRootNode::create();
    GLContainer* container = GLContainer::create(root, sf::Vector2f(0, 0), sf::Vector2f(1000, 1000));
    GLOutlinedRectangle* rect = GLOutlinedRectangle::create(container, sf::Vector2f(10, 10), sf::Vector2f(500, 100));
    rect->set_fill_color(sf::Color::Red);
    rect->set_outline_color(sf::Color::Blue);
    rect->set_outline_thickness(-10);

    GLText* text = GLText::create(container, Graphics().main_font(), 10u, "The quick brown fox");
    // text->set_char_size(20);

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
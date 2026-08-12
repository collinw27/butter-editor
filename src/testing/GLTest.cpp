#include <utility>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <gl/glew.h>
#include <SFML/OpenGL.hpp>

#include "utility/FileManager.h"
#include "utility/Graphics.h"
#include "utility/Logger.h"
#include "utility/core.h"
#include "graphics/nodes.h"

int main()
{
    try {

    FileManagerSingleton* file_manager = new FileManagerSingleton();
    GraphicsSingleton* graphics = new GraphicsSingleton();
    LoggerSingleton* logger = new LoggerSingleton();

    Graphics().init(sf::VideoMode({1280, 720}), "GLTest", sf::Style::Default);
    Graphics().set_clear_color(sf::Color(50, 50, 50));

    GLRootNode* root = GLRootNode::create();

    GLText* text_1 = GLText::create(root, Graphics().main_font(), 10u, "The quick brown fox");
    text_1->enable_special_formatting();
    text_1->add_color(sf::Color::Red, 1);
    text_1->add_color(sf::Color::Green);
    text_1->add_string(" jumps over the lazy dog.");
    text_1->set_position(sf::Vector2f(10, 10));
    text_1->set_char_size(40u);
    text_1->set_color(sf::Color::Blue);

    GLTexture* tex_1 = new GLTexture(file_manager->get_res_path("tex/playhead.png"));
    GLSprite* sprite = GLSprite::create(root, tex_1, sf::Vector2f(300, 300));
    GLSprite* sprite_small = GLSprite::create(root, tex_1, sf::Vector2f(350, 300));
    sprite_small->set_scale(sf::Vector2f(0.5, 0.5));
    GLSprite* sprite_large = GLSprite::create(root, tex_1, sf::Vector2f(400, 300));
    sprite_large->set_scale(sf::Vector2f(2.5, 2.5));
    GLTexture* tex_2 = new GLTexture(file_manager->get_res_path("tex/playhead.png"));
    tex_2->set_linear_filter(false);
    GLSprite* sprite_nearest = GLSprite::create(root, tex_2, sf::Vector2f(500, 300));
    sprite_nearest->set_scale(sf::Vector2f(2.5, 2.5));

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
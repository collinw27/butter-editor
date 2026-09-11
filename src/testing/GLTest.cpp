#include <utility>
#include <iostream>
#include <cmath>

#include <SFML/Graphics.hpp>
#include <gl/glew.h>
#include <SFML/OpenGL.hpp>

#include "utility/FileManager.h"
#include "utility/Graphics.h"
#include "utility/Logger.h"
#include "utility/core.h"
#include "graphics/nodes.h"
#include "graphics/GLFrameBuffer.h"
#include "graphics/texture/GLBufferTexture.h"

int main()
{
    try {

    FileManagerSingleton* file_manager = new FileManagerSingleton();
    GraphicsSingleton* graphics = new GraphicsSingleton();
    LoggerSingleton* logger = new LoggerSingleton();

    Graphics().init(sf::VideoMode({1280, 720}), "GLTest", sf::Style::Default);
    Graphics().set_clear_color(sf::Color(50, 50, 50));

    sf::Clock frame_clock;
    frame_clock.start();

    GLWindowNode* root = GLWindowNode::create();

    GLText* text_1 = GLText::create(root, Graphics().main_font(), 10u, "The quick brown fox");
    text_1->enable_special_formatting();
    text_1->add_color(sf::Color::Red, 1);
    text_1->add_color(sf::Color::Green);
    text_1->add_string(" jumps over the lazy dog.");
    text_1->set_position(sf::Vector2f(10, 10));
    text_1->set_char_size(40u);
    text_1->set_color(sf::Color::Blue);

    GLFrameBuffer* framebuffer = GLFrameBuffer::create(sf::Vector2u(100, 100));
    const GLBufferTexture* framebuffer_tex = framebuffer->get_texture();
    GLSprite* framebuffer_spr = GLSprite::create(root, framebuffer_tex, sf::Vector2f(100, 100));
    
    Graphics().framebuffer_set_active(framebuffer, true);
    GLRectangle* test_rect = GLRectangle::create(nullptr, sf::Vector2f(0, 0), sf::Vector2f(50, 50));
    test_rect->set_fill_color(sf::Color::Magenta);
    Graphics().framebuffer_set_active(framebuffer, false);

    sf::RenderWindow& window = Graphics().get_window();
    while (window.isOpen())
    {
        float elapsed = frame_clock.getElapsedTime().asSeconds();

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
                Graphics().on_window_resized(root);
        }

        if (elapsed >= 2 && !test_rect->get_parent())
            framebuffer->add_child(test_rect);
        // test_rect->set_position(sf::Vector2f(25, 20 + 15 * sin(elapsed * 10.f)));
        // framebuffer_spr->set_scale(sf::Vector2f(1, 1) * float(1 + 0.5 * sin(elapsed * 4.1f)));
        
        Graphics().render_framebuffer(framebuffer);
        Graphics().display(root);
    }
    }
    catch (const ButterException& e)
    {
        e.print();
    }
}
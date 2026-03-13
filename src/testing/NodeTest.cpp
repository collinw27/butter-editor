#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

#include "utility/Input.h"
#include "utility/Logger.h"
#include "graphics/RectNode.h"

// In this test, there are 3 rectangles in a linear hierarchy
// You can arbitrarily select and scale each rectangle
// This allows testing to make sure the children rectangles scale as intended

class NodeTest
{
    sf::RenderWindow *window;
    std::vector<RectNode*> nodes;

    int target = 0;

public:

    NodeTest()
    {
        window = new sf::RenderWindow(sf::VideoMode({1280, 720}), "Butter Video Editor", sf::Style::Close);

        for (int i = 0; i < 3; ++i)
        {
            RectNode *rect = new RectNode(
                (i == 0) ? nullptr : nodes.back(),
                (i == 0) ? sf::Color::Red : ((i == 1) ? sf::Color::Green : sf::Color::Blue),
                sf::FloatRect({0.f, 0.f}, {200.f, 200.f})
            );
            nodes.push_back(rect);
        }
    }

    ~NodeTest()
    {
        delete window;
        for (auto n : nodes)
        {
            delete n;
        }
    }
    
    void run()
    {
        while (window->isOpen())
        {
            Input().clear_keys();

            while (const std::optional event = window->pollEvent())
            {
                if (event->is<sf::Event::Closed>())
                    window->close();
                else if (const auto *key_pressed = event->getIf<sf::Event::KeyPressed>())
                    Input().add_key_press(key_pressed->code);
            }

            update_shapes();

            window->clear();
            draw(*window);
            window->display();
        }
    }

    void draw(sf::RenderWindow& window)
    {
        for (auto n : nodes)
        {
            n->draw(window);
        }
    }

    void update_shapes()
    {
        // Enter switches between rects

        if (Input().check_key_press(sf::Keyboard::Key::Enter))
        {
            target = (target + 1) % 3;
            Logger().log(std::stringstream{} << "Switched target: " << target);
        }

        // Arrow keys move position when SHIFT is not held

        bool up = Input().check_key_press(sf::Keyboard::Key::Up);
        bool down = Input().check_key_press(sf::Keyboard::Key::Down);
        bool left = Input().check_key_press(sf::Keyboard::Key::Left);
        bool right = Input().check_key_press(sf::Keyboard::Key::Right);
        bool scaling = Input().check_key(sf::Keyboard::Key::LShift);
        if (!scaling && (up || down || left || right))
        {
            RectNode *rect = nodes.at(target);
            sf::Vector2f delta (0, 0);
            if (left || right)
            {
                delta.x = (left) ? -1 : 1;
            }
            if (up || down)
            {
                delta.y = (up) ? -1 : 1;
            }
            rect->set_position(rect->get_position() + 10.f * delta);

            Logger().log(std::stringstream{} << "Position = " << Logger().str(rect->get_position()).str());
        }
        if (scaling && (up || down || left || right))
        {
            RectNode *rect = nodes.at(target);
            sf::Vector2f delta (1, 1);
            if (left || right)
            {
                delta.x = (left) ? 0.8 : 1.2;
            }
            if (up || down)
            {
                delta.y = (up) ? 0.8 : 1.2;
            }
            rect->set_scale(rect->get_scale().componentWiseMul(delta));

            Logger().log(std::stringstream{} << "Scale = " << Logger().str(rect->get_scale()).str());
        }

        // Reset if necessary

        if (Input().check_key_press(sf::Keyboard::Key::R))
        {
            for (auto rect : nodes)
            {
                rect->set_position(sf::Vector2f());
                rect->set_scale(sf::Vector2f(1, 1));
            }
        }
    }
};

int main()
{
    LoggerSingleton *logger = new LoggerSingleton();
    InputSingleton *input_manager = new InputSingleton();

    NodeTest* node_test = new NodeTest();
    node_test->run();

    delete logger;
    delete input_manager;

    return 0;
}
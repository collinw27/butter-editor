#include "editor/DragMouse.h"

#include <SFML/Graphics.hpp>

class DragMedia : public DragMouse
{
    sf::RectangleShape icon;

public:

    DragMedia(sf::Vector2i source_pos, sf::Color color);
    sf::Color get_color();
    virtual void on_move(sf::Vector2i mouse_pos) override;
    virtual void draw(sf::RenderWindow& window) override;
};
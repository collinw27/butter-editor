#ifndef CONFIG_MODULE_H
#define CONFIG_MODULE_H

#include <string>
#include <vector>

#include "editor/EditorModule.h"

class ConfigModule : public EditorModule
{
    struct MediaWidget
    {
        std::string name;
        sf::Color color;
        sf::RectangleShape bg;
        sf::RectangleShape col_preview;
        sf::Text name_text;
    };

    std::vector<MediaWidget> media_widgets;

    void create_media(const std::string &name, sf::Color color);

public:

    ConfigModule(Editor &editor);
    virtual void draw(sf::RenderWindow &window) override;
    
    virtual void on_mouse_moved(sf::Vector2i position, bool focused) override;
    virtual void on_mouse_pressed(sf::Vector2i position, bool focused) override;
};

#endif
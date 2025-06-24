#include "editor/config/ConfigModule.h"

#include "utility/ResourceManager.h"
#include "editor/Editor.h"
#include "editor/config/DragMedia.h"

void ConfigModule::create_media(const std::string &name, sf::Color color)
{
    sf::RectangleShape bg{sf::Vector2f({(float)bounds.size.x, 30.f})};
    bg.setPosition(sf::Vector2f(bounds.position) + sf::Vector2f(0, media_widgets.size() * 35));
    bg.setFillColor(sf::Color::Transparent);

    sf::RectangleShape col_preview{sf::Vector2f({20, 20})};
    col_preview.setPosition(bg.getPosition() + sf::Vector2f(5, 5));
    col_preview.setFillColor(color);

    sf::Text name_text{ResourceManager::singleton()->get_font(), name};
    name_text.setPosition(bg.getPosition() + sf::Vector2f(35, 3));
    name_text.setCharacterSize(20);
    name_text.setFillColor(sf::Color::White);

    media_widgets.push_back({name, color, bg, col_preview, name_text});
}

ConfigModule::ConfigModule(Editor &editor) :
    EditorModule{editor, sf::IntRect({640, 0}, {1280-640, 360})}
{
    // Populate with sample media for now :)

    create_media("Media 1", sf::Color::Red);
    create_media("Media 2", sf::Color::Green);
    create_media("Media 3", sf::Color::Blue);
}

void ConfigModule::draw(sf::RenderWindow &window)
{
    window.draw(visible_rect);
    for (auto &media : media_widgets)
    {
        window.draw(media.bg);
        window.draw(media.col_preview);
        window.draw(media.name_text);
    }
}

void ConfigModule::on_mouse_moved(sf::Vector2i position, bool focused)
{
    for (auto &media : media_widgets)
    {
        media.bg.setFillColor((media.bg.getGlobalBounds().contains(sf::Vector2f(position))) ?
            sf::Color(120, 120, 120) : sf::Color::Transparent);
    }
}

void ConfigModule::on_mouse_pressed(sf::Vector2i position, bool focused)
{
    for (auto &media : media_widgets)
    {
        if (media.bg.getGlobalBounds().contains(sf::Vector2f(position)))
        {
            editor.new_mouse_event(new DragMedia(position, media.color));
            break;
        }
    }
}
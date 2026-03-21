#include "editor/EditorModule.h"

#include <string>
#include <SFML/Graphics.hpp>

class TestModule : public EditorModule
{
    sf::Text* text;

public:

    TestModule(Editor& editor, std::string sample_text);
    ~TestModule();

    virtual void draw(sf::RenderWindow& window) override;
    virtual void set_bounds(const sf::IntRect& bounds) override;
    virtual void set_ui_scale(float new_scale) override;
};
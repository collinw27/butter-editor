#include "editor/EditorModule.h"

#include <string>
#include "graphics/nodes.h"

class TestModule : public EditorModule
{
    GLText* text;

public:

    TestModule(Editor& editor, std::string sample_text);
    ~TestModule();

    virtual void set_bounds(const sf::IntRect& bounds) override;
    virtual void set_ui_scale(float new_scale) override;
};
#ifndef TEST_MODULE_H
#define TEST_MODULE_H

#include "editor/EditorModule.h"

#include <string>
#include "graphics/nodes.h"

class TestModule : public EditorModule
{
    GLText* text;

public:

    TestModule(Editor& editor, std::string sample_text);

    virtual void set_bounds(const sf::IntRect& bounds) override;
    virtual void set_ui_scale(float new_scale) override;
};

#endif
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

    virtual void apply_ui_scale() override;
};

#endif
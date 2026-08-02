#ifndef DEBUG_MODULE_H
#define DEBUG_MODULE_H

#include "editor/core/EditorModule.h"

#include <string>
#include "graphics/nodes.h"

class DebugModule : public EditorModule
{
    GLText* text;

public:

    DebugModule(Editor& editor);

    virtual void apply_ui_scale() override;
    void refresh_info(std::string new_info);
};

#endif
#ifndef PROJECT_MODULE_H
#define PROJECT_MODULE_H

#include "editor/core/EditorModule.h"

#include <string>
#include "graphics/nodes.h"

class ProjectModule : public EditorModule
{
    GLText* text;

public:

    ProjectModule(Editor& editor);

    virtual void apply_ui_scale() override;
    void refresh_info();
};

#endif
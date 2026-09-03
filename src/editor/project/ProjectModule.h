#ifndef PROJECT_MODULE_H
#define PROJECT_MODULE_H

#include "editor/core/EditorModule.h"

#include <string>
#include "graphics/nodes.h"

class ProjectModule : public EditorModule
{
    std::unique_ptr<GLText> text;

public:

    ProjectModule(Editor& editor);

    virtual void apply_ui_scale() override;
    virtual void on_notif(int notif_class, int notif_type, size_t num_args, void** arg_ptrs) override;
    void refresh_info();
};

#endif
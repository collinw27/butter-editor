#ifndef TIMELINE_MODULE
#define TIMELINE_MODULE

#include "editor/EditorModule.h"

#include "graphics/nodes.h"

class TimelineModule : public EditorModule
{
    std::vector<GLRectangle*> clips;

public:

    TimelineModule(Editor& editor);
    ~TimelineModule();

    virtual void apply_bounds() override;
    virtual void apply_ui_scale() override;

    void refresh_clips();
};

#endif
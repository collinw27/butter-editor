#ifndef EDITOR_PANE_H
#define EDITOR_PANE_H

#include "graphics/nodes.h"

class Editor;

class EditorModule
{
protected:

    Editor& editor;
    sf::IntRect bounds;
    float ui_scale = 1.f;
    GLOutlinedRectangle* visible_rect;
    bool mouse_hover;

public:

    EditorModule(Editor& editor);
    virtual void update() {}
    GLNode* get_node();

    virtual void set_bounds(const sf::IntRect& new_bounds);
    virtual void set_ui_scale(float new_scale);
    
    void set_visible(bool visible);
    void set_hover_highlight(bool hover);
    sf::IntRect get_bounds();
};

#endif
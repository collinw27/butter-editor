#ifndef DRAG_MEDIA_H
#define DRAG_MEDIA_H

#include "editor/core/DragMouse.h"

class DragMedia : public DragMouse
{
    sf::Color media_color;

public:

    DragMedia(sf::Color color);
    sf::Color get_color();

    virtual void create_node(GLNode* parent) override;
};

#endif
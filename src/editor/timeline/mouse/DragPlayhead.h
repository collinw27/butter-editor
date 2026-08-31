#ifndef DRAG_PLAYHEAD_H
#define DRAG_PLAYHEAD_H

#include "editor/core/mouse/DragMouseEvent.h"

class DragPlayhead : public DragMouseEvent
{
public:

    DragPlayhead(EditorModule* target_module);
};

#endif
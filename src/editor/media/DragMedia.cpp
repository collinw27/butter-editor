#include "editor/media/DragMedia.h"

#include "graphics/GLRectangle.h"

DragMedia::DragMedia(id_s media_id, GLTexture* thumbnail_tex)
    : media_id{media_id}, thumbnail_tex{thumbnail_tex}
{
    this->thumbnail_tex = thumbnail_tex;
}

void DragMedia::create_node(GLNode* parent)
{
    // Height scaled down to 40
    // Width can be arbitrarily large/small

    GLSprite* node = GLSprite::create(parent, thumbnail_tex);
    node->set_scale(sf::Vector2f(40, 40) / thumbnail_tex->get_size().y);
    visible_node = node;
}
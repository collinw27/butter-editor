#include "graphics/GLContainer.h"

#include "utility/Graphics.h"
#include "utility/Logger.h"

GLContainer::GLContainer(GLNode* parent, sf::Vector2f position, sf::Vector2f size)
    : GLNode(parent)
{
    this->position = position;
    this->size = size;
}

GLContainer* GLContainer::create(GLNode* parent, sf::Vector2f position, sf::Vector2f size)
{
    GLContainer* instance = new GLContainer(parent, position, size);
    instance->init();
    return instance;
}

void GLContainer::draw()
{
    // Draw bounds are worked out by transforming corner points
    // using the global matrix
    // This doesn't account for transformations that alter angles, but
    // this class wasn't built to support such transformations anyway
    // Also, note that glScissor starts at bottom-left, so we need to do
    // a slight modification

    glm::mat4 world_to_screen = Graphics().world_to_screen();
    float window_height = (float)Graphics().get_window().getSize().y;
    glm::vec4 point_a = world_to_screen * global_matrix * glm::vec4(0, 0, 0, 1);
    glm::vec4 point_b = world_to_screen * global_matrix * glm::vec4(size.x, -size.y, 0, 1);
    sf::IntRect bounds = sf::IntRect(sf::Vector2i(point_a.x, window_height + point_b.y), sf::Vector2i(point_b.x - point_a.x, point_a.y - point_b.y));
    unsigned int check_index = Graphics().push_scissor(bounds);
    GLNode::draw();
    Graphics().pop_scissor(check_index);
}

sf::Vector2f GLContainer::get_size()
{
    return size;
}

void GLContainer::set_size(sf::Vector2f size)
{
    this->size = size;
}
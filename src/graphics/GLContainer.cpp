#include "graphics/GLContainer.h"

#include "utility/Graphics.h"

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

    glm::mat4 world_to_screen = Graphics().world_to_screen();
    glm::vec4 point_a = world_to_screen * glm::vec4(position.x, position.y, 0, 1) * global_matrix;
    glm::vec4 point_b = world_to_screen * glm::vec4(position.x + size.x, position.y + size.y, 0, 1) * global_matrix;
    sf::IntRect bounds = sf::IntRect(sf::Vector2i(point_a.x, point_a.y), sf::Vector2i(point_b.x - point_a.x, point_b.y - point_a.y));
    unsigned check_index = Graphics().push_scissor(bounds);
    GLNode::draw();
    Graphics().pop_scissor(check_index);
}
#ifndef BUTTERGL_SPRITE_H
#define BUTTERGL_SPRITE_H

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "graphics/GLNode.h"
#include "graphics/GLTexture.h"

class GLSprite : public GLNode
{
    GLTexture* texture;
    
    // Sprites use size the same way rectangles do
    // The difference is that size cannot be modified externally and
    // always takes the size of the linked texture
    
    sf::Vector2f size {1, 1};

    GLuint shader_program;
    GLuint VAO;
    GLuint vertex_VBO;
    GLuint index_VBO;

    glm::mat4 u_model_mat;

protected:
    
    GLSprite(GLNode* parent, GLTexture* texture, sf::Vector2f position);
    virtual void init() override;

public:

    static GLSprite* create(GLNode* parent, GLTexture* texture, sf::Vector2f position = {0, 0});

protected:

    virtual void on_window_resized() override;
    virtual void draw() override;
    virtual void apply_global_matrix() override;

public:

    sf::Vector2f get_size();

private:

    void setup_GL();
    void update_model_matrix();
};

#endif
#include "editor/timeline/TimelineClip.h"

#include "editor/timeline/graphics/ClipThumbnailShader.h"

TimelineClip::TimelineClip(id_s clip_id, GLNode* container)
    : clip_id{clip_id}
{
    node.reset(GLNode::create(container));
    rect.reset(GLRectangle::create(
        node.get(),
        sf::Vector2f(0, 0),
        sf::Vector2f(0, 100)
    ));
    thumbnail.reset(GLShaderRectangle::create(
        node.get(),
        new ClipThumbnailShader(),
        sf::Vector2f(6, 20),
        sf::Vector2f(0, 74)
    ));
}

GLNode* TimelineClip::get_node()
{
    return node.get();
}

GLRectangle* TimelineClip::get_rect()
{
    return rect.get();
}

GLShaderRectangle* TimelineClip::get_thumbnail()
{
    return thumbnail.get();
}

GLRectangle* TimelineClip::get_border()
{
    return border.get();
}

bool TimelineClip::selected()
{
    return is_selected;
}

void TimelineClip::render_selected(GLNode* container)
{
    if (!is_selected)
    {
        is_selected = true;
        border.reset(GLRectangle::create(
            container,
            node->get_position() - sf::Vector2f(4 / t_scale, 4),
            rect->get_size() + sf::Vector2f(8 / t_scale, 8)
        ));
        border->set_fill_color(sf::Color{255, 238, 125});
    }
    else
    {
        border->set_position(node->get_position() - sf::Vector2f(4 / t_scale, 4));
        border->set_size(rect->get_size() + sf::Vector2f(8 / t_scale, 8));
    }
}

void TimelineClip::deselect()
{
    if (is_selected)
    {
        is_selected = false;
        border->orphan();
        border.reset(nullptr);
    }
}

bool TimelineClip::is_time_within(int time)
{
    return (time >= clip_start && time < (clip_start + clip_length));
}

bool TimelineClip::is_start_within(float left, float right)
{
    return (left <= (float) clip_start && right > (float) clip_start);
}

bool TimelineClip::is_end_within(float left, float right)
{
    return (left <= (float) (clip_start + clip_length) && right > (float) (clip_start + clip_length));
}

void TimelineClip::set_t_scale(float t_scale)
{
    this->t_scale = t_scale;
    thumbnail->set_position(sf::Vector2f(6 / t_scale, thumbnail->get_position().y));
    update_length_visuals();
    if (is_selected)
    {
        border->set_position(node->get_position() - sf::Vector2f(4 / t_scale, 4));
        border->set_size(rect->get_size() + sf::Vector2f(8 / t_scale, 8));
    }
}

void TimelineClip::set_hovering(bool hovering)
{
    if (hovering)
        rect->set_fill_color(sf::Color(
            std::min((int) thumbnail_color.r + 12, 255),
            std::min((int) thumbnail_color.g + 12, 255),
            std::min((int) thumbnail_color.b + 12, 255)
        ));
    else
        rect->set_fill_color(thumbnail_color);
}

void TimelineClip::set_clip_start(VideoTime start)
{
    clip_start = start;
    node->set_position(sf::Vector2f(clip_start, node->get_position().y));
}

void TimelineClip::set_clip_length(VideoTime length)
{
    clip_length = length;
    update_length_visuals();
}

void TimelineClip::set_clip_end(VideoTime end)
{
    if (end > clip_start)
        clip_length = end - clip_start;
    node->set_position(sf::Vector2f(clip_start, node->get_position().y));
    update_length_visuals();
}

void TimelineClip::set_bg_color(sf::Color color)
{
    thumbnail_color = color;
    rect->set_fill_color(thumbnail_color);
}

void TimelineClip::set_thumbnail_texture(const GLTexture* texture)
{
    ((ClipThumbnailShader*) thumbnail->get_shader())->texture = texture;
}

void TimelineClip::update_length_visuals()
{
    rect->set_size(sf::Vector2f(clip_length, rect->get_size().y));
    thumbnail->set_size(sf::Vector2f(std::max<float>(0, rect->get_size().x - 12.0 / t_scale), thumbnail->get_size().y));

    // Now, we get the thumbnail to tile correctly
    // First, get the width of the clip's thumbnail region on the screen
    // (i.e. taking into account the scale factor)
    // Then, choose a `x_shrink` for the shader so that the aspect ratio is preserved
    // Note: `x_shrink = 2` halves the width of the texture

    float real_region_width = thumbnail->get_size().x * t_scale;
    ClipThumbnailShader* shader_program = (ClipThumbnailShader*) thumbnail->get_shader();
    if (shader_program->texture != nullptr)
    {
        sf::Vector2f tex_size = shader_program->texture->get_size();
        float aspect_ratio = (tex_size.y / tex_size.x);
        shader_program->x_shrink = aspect_ratio * (real_region_width / thumbnail->get_size().y);
    }
}
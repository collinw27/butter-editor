#version 330 core

in vec2 uv_pos;
uniform vec4 fill_color;
uniform vec4 outline_color;
uniform vec2 outline_width;

void main()
{
    if (uv_pos.x > outline_width.x && uv_pos.x < 1.0 - outline_width.x && uv_pos.y > outline_width.y && uv_pos.y < 1.0 - outline_width.y)
        gl_FragColor = fill_color;
    else
        gl_FragColor = outline_color;
}
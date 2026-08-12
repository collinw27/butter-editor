#version 330 core

in vec2 tex_coords;
uniform sampler2D tex;

void main()
{
    gl_FragColor = texture(tex, tex_coords);
}
#version 330 core

in vec2 tex_coords;
uniform sampler2D tex;
uniform float x_shrink;

void main()
{
    gl_FragColor = texture(tex, tex_coords * vec2(x_shrink, 1.0));
}
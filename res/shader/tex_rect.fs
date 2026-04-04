#version 330 core

in vec2 tex_coords;
uniform sampler2D texture;

void main()
{
    gl_FragColor = texture(texture, tex_coords);
}
#version 330 core

in vec2 tex_coords;
uniform vec3 text_color;
uniform sampler2D glyph;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(glyph, tex_coords).r);
    gl_FragColor = vec4(text_color, 1.0) * sampled;
}
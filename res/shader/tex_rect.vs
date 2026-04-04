#version 330 core

// This shader is built to transform a rectangle whose vertices
// are defined to take up the entire screen space

layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec2 tex_pos;
uniform mat4 model;
out vec2 tex_coords;

void main()
{
    gl_Position = model * vec4(vert_pos.xyz, 1.0);
    tex_coords = tex_pos;
}
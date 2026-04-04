#version 330 core

layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec2 uv_input;

uniform mat4 model;
out vec2 uv_pos;

void main()
{
    gl_Position = model * vec4(vert_pos.x, vert_pos.y, vert_pos.z, 1.0);
    uv_pos = uv_input;
}
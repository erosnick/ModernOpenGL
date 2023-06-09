#version 460 core

layout (location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoords;

layout (location = 0) out vec2 uv;

void main()
{
    uv = aTexcoords;
    gl_Position = vec4(aPosition, 1.0);
}
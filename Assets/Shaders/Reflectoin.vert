#version 460 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexcoord;

layout (location = 0) out vec3 normal;
layout (location = 1) out vec3 position;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;

void main()
{
    normal = mat3(transpose(inverse(model))) * inNormal;
    position = vec3(model * vec4(inPosition, 1.0));
    gl_Position = projection * view * model * vec4(inPosition, 1.0);
}
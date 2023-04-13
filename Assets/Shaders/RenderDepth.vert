#version 460 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexcoord;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 lightSpaceMatrix;

void main ()
{
  gl_Position = lightSpaceMatrix * model * vec4(inPosition, 1.0);
}

#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoord;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 lightSpaceMatrix;

void main ()
{
  gl_Position = lightSpaceMatrix * model * vec4(aPosition, 1.0);
}

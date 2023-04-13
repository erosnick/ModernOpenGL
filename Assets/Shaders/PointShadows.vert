#version 460 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexcoord;

layout (location = 0) out VSOut
{
  vec3 fragPosition;
  vec3 normal;
  vec2 uv;
} vsOut;

// layout(binding = 0) uniform UniformBuffer
// {
  // uniform mat4 model;
  // uniform mat4 projection;
  // uniform mat4 view;
// };

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 projection;
layout (location = 2) uniform mat4 view;
layout (location = 11) uniform bool reverseNormals;

void main ()
{
  gl_Position = projection * view * model * vec4(inPosition, 1.0);
  vsOut.fragPosition = (model * vec4(inPosition, 1.0)).xyz;

  // a slight hack to make sure the outer large cube displays lighting from the 'inside' instead of the default 'outside'.
  if (reverseNormals)
  {
    vsOut.normal = transpose(inverse(mat3(model))) * (-1.0 * inNormal);
  }
  else
  {
    vsOut.normal = transpose(inverse(mat3(model))) * inNormal;
  }
  vsOut.uv = inTexcoord;
}

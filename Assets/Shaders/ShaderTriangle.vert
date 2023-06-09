#version 460 core

#extension GL_ARB_separate_shader_objects : enable
// #extension GL_NV_uniform_buffer_std430_layout : enable
#extension GL_EXT_scalar_block_layout : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexcoord;

// layout(location = 0) out vec3 normal;
// layout(location = 1) out vec2 uv;

layout (location = 0) out VSOut
{
  vec3 fragPosition;
  vec4 fragPositionLightSpace;
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
layout (location = 3) uniform mat4 lightSpaceMatrix;

void main ()
{
  gl_Position = projection * view * model * vec4(inPosition, 1.0);
  vsOut.fragPosition = (model * vec4(inPosition, 1.0)).xyz;
  vsOut.fragPositionLightSpace = (lightSpaceMatrix * vec4(vsOut.fragPosition, 1.0));
  vsOut.normal =  transpose(inverse(mat3(model))) * inNormal;
  vsOut.uv = inTexcoord;
}

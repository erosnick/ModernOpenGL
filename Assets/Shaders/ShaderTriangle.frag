#version 460 core

#extension GL_GOOGLE_include_directive : enable

#include "Test.glsl"

// layout(location = 0) in vec3 normal;
// layout(location = 1) in vec2 uv;

layout (location = 0) in VSOut
{
  vec3 normal;
  vec2 uv;
} vsOut;

layout(location = 0) out vec4 FragColor;

layout(location = 3) uniform sampler2D albedoTexture;

layout(location = 4) uniform int useTexture;
layout(location = 5) uniform int renderDepth;
layout(location = 6) uniform int isWireframe;

void main()
{
    if (isWireframe == 1)
    {
       FragColor = vec4(0.0, 0.0, 0.0, 1.0);
       return;
    }

    vec4 ambient = vec4(0.2, 0.2, 0.2, 1.0);

    vec3 n = normalize(vsOut.normal);
    // vec3 l = normalize(vec3(0.9, 0.8, 1.0));
    vec3 l = normalize(vec3(1.0, 0.9, 0.8));

    float diffuse = clamp(dot(n, l), 0.0, 1.0);

    vec4 albedoColor = vec4(1.0);
    
    if (useTexture == 1)
    {
       albedoColor = texture(albedoTexture, vsOut.uv);
    }
    
    // FragColor = ambient + albedoColor * diffuse;
    // FragColor = vec4(vec3(diffuse), 1.0);
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);

    FragColor = albedoColor;

    if (renderDepth == 1)
    {
      float depth = albedoColor.r;
      FragColor = vec4(depth, depth, depth, 1.0);
    }
}

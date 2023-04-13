#version 460 core

layout (location = 0) out vec4 fragColor;

layout (location = 0) in vec2 texCoords;

layout (location = 0) uniform sampler2D shadowMap;
layout (location = 1) uniform float nearPlane;
layout (location = 2) uniform float farPlane;

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));	
}

void main()
{             
    float depthValue = texture(shadowMap, texCoords).r;
    fragColor = vec4(vec3(depthValue), 1.0); // orthographic
    // FragColor = vec4(vec3(LinearizeDepth(depthValue) / farPlane), 1.0); // perspective
}
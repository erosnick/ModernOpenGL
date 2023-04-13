#version 460 core

layout (location = 0) in vec4 fragPosition;

layout (location = 7) uniform vec3 lightPosition;
layout (location = 8) uniform float farPlane;

void main()
{
    // Get distance between fragment and light source
    float lightDistance = length(fragPosition.xyz - lightPosition);

    // Map to [0, 1] range by dividing by farPlane
    lightDistance = lightDistance / farPlane;

    // Write this as modified depth
    gl_FragDepth = lightDistance;
}
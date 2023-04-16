#version 460 core

#extension GL_GOOGLE_include_directive : enable

#include "Test.glsl"

// layout(location = 0) in vec3 normal;
// layout(location = 1) in vec2 uv;

layout (location = 0) in VSOut
{
  vec3 fragPosition;
  vec4 fragPositionLightSpace;
  vec3 normal;
  vec2 uv;
} vsOut;

layout(location = 0) out vec4 fragColor;

layout(location = 4, binding = 0) uniform sampler2D albedoTexture;
layout(location = 5, binding = 1) uniform sampler2D depthMap;

layout(location = 6) uniform int useTexture;
layout(location = 7) uniform int renderDepth;
layout(location = 8) uniform int isWireframe;
layout(location = 9) uniform vec3 lightPosition;
layout(location = 10) uniform vec3 viewPosition;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projectCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projectCoords = projectCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap, projectCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projectCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(vsOut.normal);
    vec3 lightDir = normalize(lightPosition - vsOut.fragPosition);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projectCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projectCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{
    if (isWireframe == 1)
    {
       fragColor = vec4(1.0, 0.0, 0.0, 1.0);
       return;
    }

    vec3 lightColor = vec3(0.5);

    // ambient
    vec3 ambient = 0.3 * lightColor;

    vec3 normal = normalize(vsOut.normal);
    // vec3 l = normalize(vec3(0.9, 0.8, 1.0));
    vec3 lightDirection = normalize(lightPosition - vsOut.fragPosition);

    vec3 diffuse = max(dot(normal, lightDirection), 0.0) * lightColor;

    // specular
    vec3 viewDirection = normalize(viewPosition - vsOut.fragPosition);
    vec3 reflectDirection = reflect(-lightDirection, normal);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);
    vec3 specular = pow(max(dot(normal, halfwayDirection), 0.0), 64.0) * lightColor;

    vec4 albedoColor = texture(albedoTexture, vsOut.uv);
    
    if (useTexture == 1)
    {
       albedoColor = texture(albedoTexture, vsOut.uv);
    }

    // calculate shadow
    float shadow = ShadowCalculation(vsOut.fragPositionLightSpace); 
    
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * albedoColor.rgb;
    fragColor = vec4(lighting, 1.0);
    // fragColor = vec4(vec3(diffuse), 1.0);
    // fragColor = vec4(vec3(shadow), 1.0);
    // fragColor = vec4(1.0, 0.0, 0.0, 1.0);

    // fragColor = albedoColor;

    if (renderDepth == 1)
    {
      float depth = texture(depthMap, vsOut.uv).r;
      fragColor = vec4(depth, depth, depth, 1.0);
    }
}

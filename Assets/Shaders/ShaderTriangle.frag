#version 460 core

#extension GL_GOOGLE_include_directive : enable

#include "Test.glsl"

// layout(location = 0) in vec3 normal;
// layout(location = 1) in vec2 uv;

const float kPi = 3.14159265;
const float kShininess = 32.0;

struct Light
{
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;
};

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
layout(location = 9) uniform vec3 viewPosition;
layout(location = 10) uniform Light lights[];

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
    vec3 lightDir = normalize(lights[0].position - vsOut.fragPosition);
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

vec3 blinnPhong(vec3 normal, vec3 lightDirection, vec3 lightColor)
{
    vec3 diffuse = max(dot(normal, lightDirection), 0.0) * lightColor;

    // specular
    vec3 viewDirection = normalize(viewPosition - vsOut.fragPosition);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);

    const float kEnergyConservation = (8.0 + kShininess ) / (8.0 * kPi); 

    // https://www.rorydriscoll.com/2009/01/25/energy-conservation-in-games/
    vec3 specular = kEnergyConservation * pow(max(dot(normal, halfwayDirection), 0.0), kShininess) * lightColor;

    return diffuse + specular;
}

float computeAttenuation(Light light, vec3 fragPosition)
{
    // attenuation
    float distance    = length(light.position - vsOut.fragPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return attenuation;
}

float spotLightIntensity(Light light, vec3 toLight)
{
    // spotlight (soft edges)
    float theta = dot(toLight, normalize(-light.direction));
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    return intensity;
}

vec3 computeSpotLight(Light light, vec3 normal, vec3 lightColor)
{
    vec3 lightDirection = normalize(light.position - vsOut.fragPosition);

    vec3 result = blinnPhong(normal, lightDirection, lightColor);

    float intensity = spotLightIntensity(light, lightDirection);

    result *= intensity;

    return result;
}

vec3 computePointLight(Light light, vec3 normal, vec3 lightColor)
{
    vec3 lightDirection = normalize(light.position - vsOut.fragPosition);

    vec3 result = blinnPhong(normal, lightDirection, lightColor);

    // attenuation
    float attenuation = computeAttenuation(light, vsOut.fragPosition);

    result *= attenuation;

    return result;
}

vec3 computeDirectionalLight(Light light, vec3 normal, vec3 lightColor)
{
    vec3 lightDirection = normalize(-light.direction);

    vec3 result = blinnPhong(normal, lightDirection, lightColor);

    return result;
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

    vec3 lighting = computeDirectionalLight(lights[0], normal, lightColor);

    // calculate shadow
    float shadow = ShadowCalculation(vsOut.fragPositionLightSpace);

    vec3 spotLight = computeSpotLight(lights[1], normal, lightColor);

    // attenuation
    float attenuation = computeAttenuation(lights[0], vsOut.fragPosition);

    lighting *= attenuation;

    vec4 albedoColor = texture(albedoTexture, vsOut.uv);
    
    if (useTexture == 1)
    {
       albedoColor = texture(albedoTexture, vsOut.uv);
    }
    
    lighting = (ambient + (1.0 - shadow) * lighting) * albedoColor.rgb;

    fragColor = vec4(lighting, 1.0);

    if (renderDepth == 1)
    {
      float depth = texture(depthMap, vsOut.uv).r;
      fragColor = vec4(depth, depth, depth, 1.0);
    }
}

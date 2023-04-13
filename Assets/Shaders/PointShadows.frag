#version 460 core

layout (location = 0) in VSOut
{
  vec3 fragPosition;
  vec3 normal;
  vec2 uv;
} vsOut;

layout(location = 0) out vec4 fragColor;

layout(location = 3, binding = 0) uniform sampler2D albedoTexture;
layout(location = 4, binding = 1) uniform samplerCube depthCubeMap;

layout(location = 5) uniform float farPlane;

layout(location = 6) uniform int useTexture;
layout(location = 7) uniform int renderDepth;
layout(location = 8) uniform int isWireframe;
layout(location = 9) uniform vec3 lightPosition;
layout(location = 10) uniform vec3 viewPosition;

float ShadowCalculation(vec3 fragPosition)
{
    // vec3 fragToLight = fragPosition - lightPosition;

    // float closestDepth = texture(depthCubeMap, fragToLight).r;
    // closestDepth *= farPlane;

    // float currentDepth = length(fragToLight);

    // float bias = 0.05;
    // float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    // fragColor = vec4(vec3(closestDepth / farPlane), 1.0);

    vec3 sampleOffsetDirections[20] = vec3[]
    (
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
          vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );

    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPosition - fragPosition);
    float diskRadius = 0.05;
    diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;

    for (int i = 0; i < samples; i++)
    {
        vec3 fragToLight = fragPosition - lightPosition;
        float currentDepth = length(fragToLight);

        float closestDepth = texture(depthCubeMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= farPlane;   // undo mapping [0, 1]

        if (currentDepth - bias > closestDepth)
        {
            shadow += 1.0;
        }
    }
    shadow /= float(samples);

    return shadow;
}

void main()
{
   if (isWireframe == 1)
    {
       fragColor = vec4(1.0, 0.0, 0.0, 1.0);
       return;
    }

    vec3 lightColor = vec3(0.3);

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

    vec4 albedoColor = vec4(1.0);
    
    if (useTexture == 1)
    {
       albedoColor = texture(albedoTexture, vsOut.uv);
    }

    // calculate shadow
    float shadow = ShadowCalculation(vsOut.fragPosition);

    // shadow = 0.0;
    
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * albedoColor.rgb;
    fragColor = vec4(vec3(lighting), 1.0);
    
    // fragColor = vec4(vec3(diffuse), 1.0);
    // fragColor = vec4(1.0, 0.0, 0.0, 1.0);

    // fragColor = albedoColor;

    if (renderDepth == 1)
    {
        vec3 direction = vec3(1.0);
        float depth = texture(depthCubeMap, direction).r;
        fragColor = vec4(depth, depth, depth, 1.0);
    }
}
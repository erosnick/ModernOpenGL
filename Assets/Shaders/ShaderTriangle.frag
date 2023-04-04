#version 460 core

// layout(location = 0) in vec3 normal;
// layout(location = 1) in vec2 uv;

layout (location = 0) in VSOut
{
  vec3 normal;
  vec2 uv;
} fsIn;

out vec4 FragColor;

uniform sampler2D albedoTexture;

uniform bool useTexture;
uniform bool renderDepth;

void main()
{
    vec4 ambient = vec4(0.2, 0.2, 0.2, 1.0);

    vec3 n = normalize(fsIn.normal);
    // vec3 l = normalize(vec3(0.9, 0.8, 1.0));
    vec3 l = normalize(vec3(1.0, 0.9, 0.8));

    float diffuse = clamp(dot(n, l), 0.0, 1.0);

    vec4 albedoColor = vec4(1.0);
    
    if (useTexture)
    {
       albedoColor = texture(albedoTexture, fsIn.uv);
    }
    
    // FragColor = ambient + albedoColor * diffuse;
    // FragColor = vec4(vec3(diffuse), 1.0);
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);

    FragColor = albedoColor;

    if (renderDepth)
    {
      float depth = albedoColor.r;
      FragColor = vec4(depth, depth, depth, 1.0);
    }
}

#version 460 core

layout (location = 0) in vec2 uv;

layout (location = 0) out vec4 FragColor;

layout (location = 0) uniform sampler2D albedo;

void main()
{
    vec3 textureColor = texture(albedo, uv).rgb;

    FragColor = vec4(textureColor, 1.0);
}
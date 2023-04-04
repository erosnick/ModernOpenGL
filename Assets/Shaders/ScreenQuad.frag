#version 460 core

in vec2 uv;

out vec4 FragColor;

uniform sampler2D albedo;

void main()
{
    vec3 textureColor = texture(albedo, uv).rgb;

    FragColor = vec4(textureColor, 1.0);
}
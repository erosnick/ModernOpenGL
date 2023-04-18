#version 460 core

layout (location = 0) out vec4 fragColor;

layout (location = 0) in vec3 normal;
layout (location = 1) in vec3 position;

layout (location = 3) uniform vec3 viewPosition;
layout (location = 4, binding = 0) uniform samplerCube skyboxTexture1;
layout (location = 5, binding = 1) uniform samplerCube skyboxTexture2;

layout (location = 6) uniform float alpha = 0.0;
layout (location = 7) uniform mat4 rotation;

void main()
{
    vec3 I = normalize(position - viewPosition);
    vec3 R = reflect(I, normalize(normal));

    R = vec3(rotation * vec4(R, 0.0));

	vec4 color1 = vec4(texture(skyboxTexture1, R).rgb, 1.0);
	vec4 color2 = vec4(texture(skyboxTexture2, R).rgb, 1.0);

	fragColor = mix(color1, color2, alpha);
}


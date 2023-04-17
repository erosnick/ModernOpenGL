#version 460 core

layout (location = 0) in vec2 texcoord;
layout (location = 1) in vec3 reflectionDirection;

layout (location = 0) out vec4 fragColor;

layout (location = 3, binding = 0) uniform samplerCube skyboxTexture1;
layout (location = 4, binding = 1) uniform samplerCube skyboxTexture2;

layout (location = 5) uniform float alpha = 0.0;


void main() {
	vec4 color1 = texture(skyboxTexture1, reflectionDirection);
	vec4 color2 = texture(skyboxTexture2, reflectionDirection);
	fragColor = mix(color1, color2, alpha);
	// fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
#version 460 core

layout (location = 0) in vec3 inPosition;
layout (location = 3) in vec3 inNormal;
layout (location = 4) in vec2 inTexcoord;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;

layout (location = 0) out vec2 texcoord;
layout (location = 1) out vec3 reflectionDirection;	// Reflected direction

void main() {

	// inPosition - origin(0, 0, 0) = inPosition
	reflectionDirection = inPosition;

	texcoord = inTexcoord;

	vec4 position = projection * view * model * vec4(inPosition, 1.0);

	gl_Position = position.xyww;
}
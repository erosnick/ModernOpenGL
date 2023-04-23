#pragma once

#include <glm/glm.hpp>

struct Light
{
	glm::vec3 position;
	glm::vec3 direction;

	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;
};
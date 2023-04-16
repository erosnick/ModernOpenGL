#pragma once

#include <glm/glm.hpp>

#include <glad/glad.h>

#include "VertexBufferLayout.h"

struct SimpleVertex
{
	SimpleVertex()
		: position{},
		normal{},
		texcoord{}
	{}

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};
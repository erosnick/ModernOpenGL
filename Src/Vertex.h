#pragma once

#include <glm/glm.hpp>

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

struct VertexElementLayout
{
	uint32_t index;
	int32_t size;
	uint32_t type;
	uint32_t offset;
};
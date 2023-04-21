#pragma once

#include <glm/glm.hpp>

#include <glad/glad.h>

struct SimpleVertex
{
	SimpleVertex()
		: position{},
		normal{},
		texcoord{}
	{}

	SimpleVertex(const glm::vec3& inPosition, const glm::vec3& inNormal, const glm::vec2& inTexcoord)
		: position(inPosition), 
		  normal(inNormal), 
		  texcoord(inTexcoord)
	{}

	SimpleVertex(float x, float y, float z, float nx, float ny, float nz, float tx, float ty)
		: position(x, y, z), normal(nx, ny, nz), texcoord(tx, ty)
	{}

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};
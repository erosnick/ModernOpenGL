#pragma once

#include <string>
#include <glm/glm.hpp>
#include <vector>

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

struct Mesh
{
	uint32_t indicesCount() const { return static_cast<uint32_t>(indices.size()); }

	void addVertex(const SimpleVertex& vertex)
	{
		vertices.emplace_back(vertex);
	}

	void addIndex(uint32_t index)
	{
		indices.push_back(index);
	}

	std::vector<SimpleVertex> vertices;
	std::vector<uint32_t> indices;

	uint32_t VAO;
	uint32_t VBO;
	uint32_t EBO;
};

struct Model
{
	void load(const std::string& path);

	std::vector<Mesh> meshes;
	uint32_t numMeshes = 0;
};
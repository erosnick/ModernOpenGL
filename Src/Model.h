#pragma once

#include <string>
#include <glm/glm.hpp>
#include <vector>

#include "Texture.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

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

	std::vector<Texture> textures;

	uint32_t numTexture = 0;

	uint32_t VAO;
	uint32_t VBO;
	uint32_t EBO;
};

struct Model
{
	void load(const std::string& path);

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);

	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	std::vector<Texture> texturesLoaded;

	std::vector<Mesh> meshes;
	uint32_t numMeshes = 0;

	std::string directory;
};
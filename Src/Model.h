#pragma once

#include <string>
#include <glm/glm.hpp>
#include <vector>

#include "Texture.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "VertexArrayObject.h"
#include "VertexBufferObjectIndexed.h"

#include "Vertex.h"

struct Mesh
{
	Mesh() { vertices = {}; };
	Mesh(const Mesh& other);

	~Mesh() {};

	void addVertex(const SimpleVertex& vertex)
	{
		vertices.emplace_back(vertex);
		numVertices++;
	}

	void addIndex(uint32_t index)
	{
		indices.push_back(index);
		numIndices++;
	}

	void createBuffers();

	std::vector<SimpleVertex> vertices;

	union
	{
		struct
		{
			std::vector<SimpleVertex> vertices;
		};

		struct
		{
			std::vector<glm::vec3> positions;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec2> texcoords;
		};
	};

	std::vector<uint32_t> indices;

	std::vector<Texture> textures;

	uint32_t numTexture = 0;
	uint32_t numVertices = 0;
	uint32_t numIndices = 0;

	VertexArrayObject VAO;
	VertexBufferObjectIndexed VBOIndexed;
};

struct Model
{
	void load(const std::string& path);

	void addMesh(const Mesh& mesh);

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);

	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	std::vector<Texture> texturesLoaded;

	std::vector<Mesh> meshes;
	uint32_t numMeshes = 0;

	std::string directory;

	glm::vec3 position{ 0.0f };
	glm::vec3 scale{ 1.0f };
	glm::vec3 rotation{ 0.0f };
};
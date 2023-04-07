#include "Model.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <iostream>

void Model::load(const std::string& path)
{
	// read file via ASSIMP
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		auto mesh = scene->mMeshes[i];

		Mesh submesh;

		for (uint32_t j = 0; j < mesh->mNumFaces; j++)
		{
			const auto& face = mesh->mFaces[j];

			submesh.addIndex(face.mIndices[0]);
			submesh.addIndex(face.mIndices[1]);
			submesh.addIndex(face.mIndices[2]);
		}

		for (uint32_t j = 0; j < mesh->mNumVertices; j++)
		{
			auto position = mesh->mVertices[j];
			auto normal = mesh->mNormals[j];

			aiVector3D texcoord = { 0.0f, 0.0f, 0.0f };

			if (mesh->mTextureCoords[0] != nullptr)
			{
				texcoord = mesh->mTextureCoords[0][j];
			}

			SimpleVertex vertex;
			vertex.position = { position.x, position.y, position.z };
			vertex.normal = { normal.x, normal.y, normal.z };
			vertex.texcoord = { texcoord.x, texcoord.y };

			submesh.addVertex(vertex);
		}

		meshes.emplace_back(submesh);
	}
}

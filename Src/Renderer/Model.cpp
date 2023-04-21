#include "AriaPCH.h"

#include "Model.h"

void Model::load(const std::string& path)
{
	// read file via ASSIMP
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		ARIA_CORE_CRITICAL("ERROR::ASSIMP:: {0}", importer.GetErrorString());
		return;
	}

	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		std::vector<Texture> textures;

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
			SimpleVertex vertex;

			auto attribute = mesh->mVertices[j];

			vertex.position = { attribute.x, attribute.y, attribute.z };

			if (mesh->HasNormals())
			{
				attribute = mesh->mNormals[j];
				vertex.normal = { attribute.x, attribute.y, attribute.z };
			}

			if (mesh->mTextureCoords[0] != nullptr)
			{
				attribute = mesh->mTextureCoords[0][j];
				vertex.texcoord = { attribute.x, attribute.y };
			}

			submesh.addVertex(vertex);
		}

		// process materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		// 1. diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "albedoTexture");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// 2. specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specularTexture");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "normalTexture");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		// 4. Metallic maps
		std::vector<Texture> metallicMaps = loadMaterialTextures(material, aiTextureType_METALNESS, "metallicTexture");
		textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());

		submesh.textures = textures;
		submesh.numTexture = static_cast<uint32_t>(textures.size());

		meshes.emplace_back(submesh);

		numMeshes++;
	}
}

void Model::addMesh(const Mesh& mesh)
{
	meshes.emplace_back(mesh);
	numMeshes++;
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString path;
		material->GetTexture(type, i, &path);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < texturesLoaded.size(); j++)
		{
			if (std::strcmp(texturesLoaded[j].path.data(), path.C_Str()) == 0)
			{
				textures.push_back(texturesLoaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;

			// Replace %20(space) with " "
			std::string modifiedPath = path.C_Str();

			auto position = modifiedPath.find("%20");

			while (position != std::string::npos)
			{
				if (position != std::string::npos)
				{
					modifiedPath.replace(position, 3, " ");
				}

				position = modifiedPath.find("%20");
			}

			texture = Texture::load(modifiedPath, directory);
			texture.typeName = typeName;
			texture.path = modifiedPath;
			textures.push_back(texture);
			texturesLoaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

Mesh::Mesh(const Mesh& other)
{
	vertices = other.vertices;
	indices = other.indices;

	textures = other.textures;

	numTexture = other.numTexture;
	numVertices = other.numVertices;
	numIndices = other.numIndices;

	VAO = other.VAO;
	VBOIndexed = other.VBOIndexed;

	VertexBufferObjectIndexed VBOIndexed;
}

void Mesh::createBuffers()
{
	VAO.create();
	VAO.bind();

	VBOIndexed.Create();

	for (size_t i = 0; i < numVertices; i++)
	{
		const auto& vertex = vertices[i];

		VBOIndexed.AddVertexData(&vertex, sizeof(SimpleVertex));
	}

	for (size_t i = 0; i < numIndices; i++)
	{ 
		auto index = indices[i];
		VBOIndexed.AddIndexData(&index, sizeof(uint32_t));
	}

	VBOIndexed.UploadDataToGPU(GL_DYNAMIC_STORAGE_BIT);

	VertexBufferLayout layout;
	layout.push<float>(3);
	layout.push<float>(3);
	layout.push<float>(2);

	VAO.addBuffer(VBOIndexed, layout);
}

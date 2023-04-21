#pragma once

#include <cstdint>
#include <vector>

class VertexBufferObjectIndexed
{
public:
	VertexBufferObjectIndexed();
	~VertexBufferObjectIndexed();

	void Create();									// Creates a VBO
	void Release();									// Releases the VBO

	void AddVertexData(const void* pVertexData, uint32_t vertexDataSize);	// Adds vertex data
	void AddIndexData(const void* pIndexData, uint32_t indexDataSize);	// Adds index data
	void UploadDataToGPU(int iUsageHint);	
	// Upload the VBO to the GPU
	uint32_t getVBO() const { return m_vboVertices; }
	uint32_t getIBO() const { return m_vboIndices; }

private:
	uint32_t m_vboVertices = 0;		// VBO id for vertices
	uint32_t m_vboIndices = 0;		// VBO id for indices

	std::vector<uint8_t> m_vertexData;	// Vertex data to be uploaded
	std::vector<uint8_t> m_indexData;	// Index data to be uploaded

	bool m_dataUploaded;		// Flag indicating if data is uploaded to the GPU
};
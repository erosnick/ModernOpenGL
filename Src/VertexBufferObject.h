#pragma once

#include <cstdint>
#include <vector>

// This class provides a wrapper around an OpenGL Vertex Buffer Object
class VertexBufferObject
{
public:
	VertexBufferObject();
	~VertexBufferObject();

	void Create();									// Creates a VBO
	void Release();									// Releases the VBO

	void AddData(void* ptrData, uint32_t dataSize);	// Adds data to the VBO
	void UploadDataToGPU(int usageHint);			// Uploads the VBO to the GPU

	uint32_t getId() const { return m_vbo; }
	
private:
	uint32_t m_vbo = 0;									// VBO id
	std::vector<uint8_t> m_data;							// Data to be put in the VBO
	bool m_dataUploaded;							// A flag indicating if the data has been sent to the GPU
};
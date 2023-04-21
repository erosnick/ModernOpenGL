#include "AriaPCH.h"

#include "VertexBufferObject.h"

#include <glad/glad.h>

// Constructor -- initialize member variable m_bDataUploaded to false
VertexBufferObject::VertexBufferObject()
{
	m_dataUploaded = false;
}

VertexBufferObject::~VertexBufferObject()
{
}

// Create a VBO 
void VertexBufferObject::Create()
{
	glCreateBuffers(1, &m_vbo);//uses DSA. This is the way.
}

// Release the VBO and any associated data
void VertexBufferObject::Release()
{
	glDeleteBuffers(1, &m_vbo);
	m_dataUploaded = false;
	m_data.clear();
}

// Uploads the data to the GPU.  Afterwards, the data can be cleared.  
// iUsageHint - GL_STATIC_DRAW, GL_DYNAMIC_DRAW...
void VertexBufferObject::UploadDataToGPU(int usageHint)
{
	glNamedBufferStorage(m_vbo, m_data.size(), &m_data[0], usageHint);

	glBufferData(GL_ARRAY_BUFFER, m_data.size(), &m_data[0], usageHint);
	m_dataUploaded = true;
	m_data.clear();
}

// Adds data to the VBO.  
void VertexBufferObject::AddData(const void* ptrData, uint32_t dataSize)
{
	m_data.insert(m_data.end(), (uint8_t*)ptrData, (uint8_t*)ptrData + dataSize);
}




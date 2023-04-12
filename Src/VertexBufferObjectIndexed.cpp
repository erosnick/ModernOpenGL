#include "VertexBufferObjectIndexed.h"

#include <glad/glad.h>

// Constructor -- initialise member variable m_bDataUploaded to false
CVertexBufferObjectIndexed::CVertexBufferObjectIndexed()
{
	m_dataUploaded = false;
}

CVertexBufferObjectIndexed::~CVertexBufferObjectIndexed()
{}

// Create buffer objects for the vertices and indices
void CVertexBufferObjectIndexed::Create()
{
	glCreateBuffers(1, &m_vboVertices);//uses DSA. This is the way.
	glCreateBuffers(1, &m_vboIndices);//uses DSA. This is the way.
}

// Release the buffers and any associated data
void CVertexBufferObjectIndexed::Release()
{
	glDeleteBuffers(1, &m_vboVertices);
	glDeleteBuffers(1, &m_vboIndices);
	m_dataUploaded = false;
	m_vertexData.clear();
	m_indexData.clear();
}

// Binds the buffers
void CVertexBufferObjectIndexed::Bind(uint32_t VAO, uint32_t size)
{
	glVertexArrayVertexBuffer(
		VAO,					// vao to bind
		0,						// Could be 1, 2... if there were several vbo to source.
		m_vboVertices,			// VBO to bound at "vaoBindingPoint".
		0,                      // offset of the first element in the buffer hctVBO.
		size);					// stride == 3 position floats + 3 color floats.

	glVertexArrayElementBuffer(VAO, m_vboIndices);
}

// Uploads the data to the GPU.  Afterwards, the data can be cleared.  
// iUsageHint - GL_STATIC_DRAW, GL_DYNAMIC_DRAW...
void CVertexBufferObjectIndexed::UploadDataToGPU(int iUsageHint)
{
	glNamedBufferStorage(m_vboVertices, m_vertexData.size(), &m_vertexData[0], iUsageHint);
	glNamedBufferStorage(m_vboIndices, m_indexData.size(), &m_indexData[0], iUsageHint);

	m_dataUploaded = true;
	m_vertexData.clear();
	m_indexData.clear();
}

// Adds data to the VBO.  
void CVertexBufferObjectIndexed::AddVertexData(const void* ptrVertexData, uint32_t uiVertexDataSize)
{
	m_vertexData.insert(m_vertexData.end(), (uint8_t*)ptrVertexData, (uint8_t*)ptrVertexData + uiVertexDataSize);
}

// Adds data to the VBO.  
void CVertexBufferObjectIndexed::AddIndexData(const void* ptrIndexData, uint32_t uiIndexDataSize)
{
	m_indexData.insert(m_indexData.end(), (uint8_t*)ptrIndexData, (uint8_t*)ptrIndexData+uiIndexDataSize);
}


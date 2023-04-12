#include "VertexBufferObject.h"

#include <glad/glad.h>

// Constructor -- initialize member variable m_bDataUploaded to false
CVertexBufferObject::CVertexBufferObject()
{
	m_dataUploaded = false;
}

CVertexBufferObject::~CVertexBufferObject()
{
}

// Create a VBO 
void CVertexBufferObject::Create()
{
	glCreateBuffers(1, &m_vbo);//uses DSA. This is the way.
}

// Release the VBO and any associated data
void CVertexBufferObject::Release()
{
	glDeleteBuffers(1, &m_vbo);
	m_dataUploaded = false;
	m_data.clear();
}


// Binds a VBO.  
void CVertexBufferObject::Bind(uint32_t VAO, uint32_t size)
{
	glVertexArrayVertexBuffer(
		VAO,					// vao to bind
		0,						// Could be 1, 2... if there were several vbo to source.
		m_vbo,			// VBO to bound at "vaoBindingPoint".
		0,                      // offset of the first element in the buffer hctVBO.
		size);					// stride == 3 position floats + 3 color floats.
}


// Uploads the data to the GPU.  Afterwards, the data can be cleared.  
// iUsageHint - GL_STATIC_DRAW, GL_DYNAMIC_DRAW...
void CVertexBufferObject::UploadDataToGPU(int usageHint)
{
	glNamedBufferStorage(m_vbo, m_data.size(), &m_data[0], usageHint);

	glBufferData(GL_ARRAY_BUFFER, m_data.size(), &m_data[0], usageHint);
	m_dataUploaded = true;
	m_data.clear();
}

// Adds data to the VBO.  
void CVertexBufferObject::AddData(void* ptrData, uint32_t dataSize)
{
	m_data.insert(m_data.end(), (uint8_t*)ptrData, (uint8_t*)ptrData + dataSize);
}




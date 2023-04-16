#include "VertexArrayObject.h"

#include <glad/glad.h>

VertexArrayObject::VertexArrayObject()
{
}

VertexArrayObject::~VertexArrayObject()
{
}

void VertexArrayObject::create()
{
	glCreateVertexArrays(1, &id);
}

void VertexArrayObject::bind() const
{
	glBindVertexArray(id);
}

void VertexArrayObject::addBuffer(const VertexBufferObjectIndexed vbo, const VertexBufferLayout& layout)
{
	bindVBO(vbo.getVBO(), vbo.getIBO(), layout.getStride());

	setVertexBufferLayout(layout);
}

void VertexArrayObject::bindVBO(uint32_t VBO, uint32_t IBO, uint32_t stride) const
{
	glVertexArrayVertexBuffer(
		id,						// vao to bind
		0,						// Could be 1, 2... if there were several vbo to source.
		VBO,					// VBO to bound at "vaoBindingPoint".
		0,                      // offset of the first element in the buffer hctVBO.
		stride);				// stride == 3 position floats + 3 color floats.

	glVertexArrayElementBuffer(id, IBO);
}

void VertexArrayObject::setVertexBufferLayout(const VertexBufferLayout& layout)
{
	auto vertexElements = layout.getVertexElements();

	uint32_t offset = 0;
	for (uint32_t i = 0; i < vertexElements.size(); i++)
	{
		const auto& element = vertexElements[i];

		glEnableVertexArrayAttrib(id, i);	// Need to precise vao, as there is no context binding in DSA style
		glVertexArrayAttribFormat(id, i, element.size, element.type, false, offset);// Need to precise vao, as there is no context binding in DSA
		
		//Explicit binding of an attribute to a vao binding point
		glVertexArrayAttribBinding(id, i, 0);

		offset += VertexBufferElement::getSizeOfType(element.type) * element.size;
	}
}

void VertexArrayObject::unbind() const
{
	glBindVertexArray(0);
}

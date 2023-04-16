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

void VertexArrayObject::setVertexElementLayout(const std::vector<VertexElementLayout> layouts)
{
	for (size_t i = 0; i < layouts.size(); i++)
	{
		glEnableVertexArrayAttrib(id, layouts[i].index);	// Need to precise vao, as there is no context binding in DSA style
		glVertexArrayAttribFormat(id, layouts[i].index, layouts[i].size, layouts[i].type, false, layouts[i].offset);// Need to precise vao, as there is no context binding in DSA
		
		//Explicit binding of an attribute to a vao binding point
		glVertexArrayAttribBinding(id, layouts[i].index, 0);
	}
}

void VertexArrayObject::unbind() const
{
	glBindVertexArray(0);
}

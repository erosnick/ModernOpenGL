#pragma once

#include <cstdint>
#include <vector>

#include "VertexBufferLayout.h"
#include "VertexBufferObjectIndexed.h"

class VertexArrayObject
{
public:
	VertexArrayObject();
	~VertexArrayObject();

	void create();

	void bind() const;
	void addBuffer(const VertexBufferObjectIndexed vbo, const VertexBufferLayout& layout);
	void bindVBO(uint32_t VBO, uint32_t IBO, uint32_t stride) const;
	void setVertexBufferLayout(const VertexBufferLayout& layout);
	void unbind() const;

	uint32_t getId() const { return id; }

private:
	uint32_t id = 0;
};
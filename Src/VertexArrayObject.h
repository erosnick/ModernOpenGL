#pragma once

#include <cstdint>
#include <vector>

#include "Vertex.h"


class VertexArrayObject
{
public:
	VertexArrayObject();
	~VertexArrayObject();

	void create();

	void bind() const;
	void setVertexElementLayout(const std::vector<VertexElementLayout> layouts);
	void unbind() const;

	uint32_t getId() const { return id; }

private:
	uint32_t id = 0;
};
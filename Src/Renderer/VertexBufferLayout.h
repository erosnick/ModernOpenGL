#pragma once

#include <cstdint>
#include <vector>

#include <glad/glad.h>

struct VertexBufferElement
{
	int32_t size;
	uint32_t type;

	inline static uint32_t getSizeOfType(GLenum type)
	{
		uint32_t size = 0;
		switch (type)
		{
		case GL_FLOAT:
			size = sizeof(GLfloat);
			break;
		case GL_UNSIGNED_INT:
			size = sizeof(GLuint);
			break;
		case GL_UNSIGNED_BYTE:
			size = sizeof(GLubyte);
			break;
		default:
			break;
		}

		return size;
	}
};

class VertexBufferLayout
{
public:
	VertexBufferLayout() {}
	~VertexBufferLayout() {}

	const std::vector<VertexBufferElement>& getVertexElements() const { return vertexElements; }

	uint32_t getStride() const { return stride; }

	template<typename T>
	void push(int32_t count);

	template<>
	void push<float>(int32_t count)
	{
		vertexElements.push_back({ count, GL_FLOAT });
		stride += sizeof(GLfloat) * count;
	}

	template<>
	void push<uint32_t>(int32_t count)
	{
		vertexElements.push_back({ count, GL_UNSIGNED_INT });
		stride += sizeof(GLuint) * count;
	}

	template<>
	void push<uint8_t>(int32_t count)
	{
		vertexElements.push_back({ count, GL_UNSIGNED_BYTE });
		stride += sizeof(GLubyte) * count;
	}

private:
	std::vector<VertexBufferElement> vertexElements;

	uint32_t stride = 0;
};
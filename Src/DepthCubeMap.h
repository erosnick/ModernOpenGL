#pragma once

#include <cstdint>
#include <array>

class DepthCubeMap
{
public:
	DepthCubeMap();

	~DepthCubeMap();
	void create(uint32_t width, uint32_t height);
	void bindFBO();
	void use(uint32_t textureUnit = 0);
	void clear(const std::array<float, 4>& clearValue = { 1.0f, 1.0f, 1.0f, 1.0 });
private:
	uint32_t FBO = 0;
	uint32_t id = 0;
};
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <GLAD/glad.h>

class Texture
{
public:
	enum class EType
	{
		Texture2D,
		TextureImage,
		TextureCube
	};

	Texture();

	void create(uint32_t inWidth, uint32_t inHeight, EType inType = EType::Texture2D, int32_t internalFormat = GL_RGBA8, int32_t format = GL_RGBA, int bpp = 4);
	void createImage(uint32_t inWidth, uint32_t inHeight);

	std::vector<float> getImageData();

	void load(const std::string& path, int bpp = 4);

	void use(uint32_t textureUnit);

	int32_t getWidth() const { return width; }
	int32_t getHeight() const { return height; }

	float getAspect() const { return static_cast<float>(width) / static_cast<float>(height); }
	
	uint32_t getId() const { return id; }

private:
	uint32_t id = 0;
	std::string name = "";
	EType type = EType::Texture2D;

	int32_t width = 0;
	int32_t height = 0;
	int32_t channels = 0;

	uint8_t* data = nullptr;
};


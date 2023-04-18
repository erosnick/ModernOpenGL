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

	static Texture load(const std::string &path, const std::string &directory = "", int bpp = 4, bool flipVertically = true);
	static Texture Texture::loadCubemap(const std::string& baseName, int32_t wrapMode = GL_CLAMP_TO_EDGE, bool hdr = false);

	const void use(uint32_t textureUnit = 0) const;

	float getAspect() const { return static_cast<float>(width) / static_cast<float>(height); }

	uint32_t getId() const { return id; }

	std::string name = "";
	std::string path = "";
	EType type = EType::Texture2D;
	std::string typeName = "";
	int32_t width = 0;
	int32_t height = 0;
	int32_t channels = 0;

private:
	uint32_t id = 0;
	uint8_t* data = nullptr;
};


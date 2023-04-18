#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <vector>
#include <filesystem>

#include "Log.h"

std::string suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };

GLenum targets[] = 
{
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

Texture::Texture()
{

}

void Texture::create(uint32_t inWidth, uint32_t inHeight, EType inType, int32_t internalFormat, int32_t format, int bpp)
{
	width = inWidth;
	height = inHeight;

	// glCreateTextures is the equivalent of glGenTextures + glBindTexture(for initialization).
	glCreateTextures(GL_TEXTURE_2D, 1, &id);

	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureStorage2D(id, 1, GL_RGBA8, inWidth, inHeight);

	std::vector<uint8_t> imageData(width * height * bpp);
	glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, imageData.data());
}

void Texture::createImage(uint32_t inWidth, uint32_t inHeight)
{
	width = inWidth;
	height = inHeight;

	// glCreateTextures is the equivalent of glGenTextures + glBindTexture(for initialization).
	glCreateTextures(GL_TEXTURE_2D, 1, &id);

	glTextureStorage2D(id, 1, GL_RGBA32F, width, height);

	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// 将纹理绑定到Shader中的image2D对象
	glBindImageTexture(0, id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glBindTextureUnit(0, id);

	//	target
	//	Specifies the target to which the texture is bound for glTexSubImage2D.Must be GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, or GL_TEXTURE_1D_ARRAY.

	//	texture
	//	Specifies the texture object name for glTextureSubImage2D.The effective target of texture must be one of the valid target values above.

	//	level
	//	Specifies the level - of - detail number.Level 0 is the base image level.Level n is the nth mipmap reduction image.

	//	xoffset
	//	Specifies a texel offset in the x direction within the texture array.

	//	yoffset
	//	Specifies a texel offset in the y direction within the texture array.

	//	width
	//	Specifies the width of the texture subimage.

	//	height
	//	Specifies the height of the texture subimage.

	//	format
	//	Specifies the format of the pixel data.The following symbolic values are accepted : GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_DEPTH_COMPONENT, and GL_STENCIL_INDEX.

	//	type
	//	Specifies the data type of the pixel data.The following symbolic values are accepted : GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV.

	//	pixels
	//	Specifies a pointer to the image data in memory.
	std::vector<float> imageData(width * height * sizeof(float));
	glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, imageData.data());
}

std::vector<float> Texture::getImageData()
{
	uint32_t collectionSize = width * height * sizeof(float);
	std::vector<float> computeData(collectionSize);
	//	texture
	//	Specifies the name of the source texture object.Must be GL_TEXTURE_1D, GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_3D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_ARRAY or GL_TEXTURE_RECTANGLE.In specific, bufferand multisample textures are not permitted.

	//	level
	//	Specifies the level - of - detail number.Level 0 is the base image level.Level n is the nth mipmap reduction image.

	//	xoffset
	//	Specifies a texel offset in the x direction within the texture array.

	//	yoffset
	//	Specifies a texel offset in the y direction within the texture array.

	//	zoffset
	//	Specifies a texel offset in the z direction within the texture array.

	//	width
	//	Specifies the width of the texture subimage.

	//	height
	//	Specifies the height of the texture subimage.

	//	depth
	//	Specifies the depth of the texture subimage.

	//	format
	//	Specifies the format of the pixel data.The following symbolic values are accepted : GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_DEPTH_COMPONENTand GL_STENCIL_INDEX.

	//	type
	//	Specifies the data type of the pixel data.The following symbolic values are accepted : GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV.

	//	bufSize
	//	Specifies the size of the buffer to receive the retrieved pixel data.

	//	pixels
	//	Returns the texture subimage.Should be a pointer to an array of the type specified by type.
	glGetTextureSubImage(id, 0, 0, 0, 0,width, height, 1, GL_RGBA, GL_FLOAT, static_cast<uint32_t>(computeData.size() * sizeof(float)), computeData.data());

	return computeData;
}

Texture Texture::load(const std::string &path, const std::string &directory, int bpp, bool flipVertically)
{
	Texture texture;

	std::string filename = std::string(path);
	  
	if (!directory.empty())
	{
		filename = directory + '/' + filename;
	}

	// glCreateTextures is the equivalent of glGenTextures + glBindTexture(for initialization).
	glCreateTextures(GL_TEXTURE_2D, 1, &texture.id);

	glTextureParameteri(texture.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(texture.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(texture.id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(texture.id, GL_TEXTURE_WRAP_T, GL_REPEAT);

	stbi_set_flip_vertically_on_load(flipVertically);

	uint8_t* data = stbi_load(filename.c_str(), &texture.width, &texture.height, &texture.channels, bpp);
	//uint8_t* data = stbi_load("./Assets/Textures/globe.jpg", &width, &height, &channels, 4);

	// When target is GL_TEXTURE_2D, GL_PROXY_TEXTURE_2D, GL_TEXTURE_RECTANGLE, GL_PROXY_TEXTURE_RECTANGLE or GL_PROXY_TEXTURE_CUBE_MAP, 
	// calling glTexStorage2D is equivalent, assuming no errors are generated, to executing the following pseudo - code:

	//for (i = 0; i < levels; i++) {
	//	glTexImage2D(target, i, internalformat, width, height, 0, format, type, NULL);
	//	width = max(1, (width / 2));
	//	height = max(1, (height / 2));
	//}

	// 根据上面的文档，第二个参数levels必须大于等于1，和下面glTextureSubImage2D的levels(mipmap levels)参数含义不同
	if (bpp == 3)
	{
		glTextureStorage2D(texture.id, 1, GL_RGB8, texture.width, texture.height);
		glTextureSubImage2D(texture.id, 0, 0, 0, texture.width, texture.height, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		glTextureStorage2D(texture.id, 1, GL_RGBA8, texture.width, texture.height);
		glTextureSubImage2D(texture.id, 0, 0, 0, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	stbi_image_free(data);

	return texture;
}

Texture Texture::loadCubemap(const std::string& baseName, int32_t wrapMode, bool hdr)
{
	Texture texture;

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &texture.id);

	bool firstTime = true;

	for (int i = 0; i < 6; i++) 
	{
		std::string ext = ".png";

		if (hdr) 
		{
			ext = ".hdr";
		}

		std::string textureName = std::string(baseName) + "_" + suffixes[i] + ext;

		if (!std::filesystem::exists(textureName)) 
		{
			ARIA_CORE_CRITICAL("Texture {0} not found.", textureName);
			return {};
		}

		int32_t bpp = 0;
		uint8_t* data = stbi_load(textureName.c_str(), &texture.width, &texture.height, &bpp, 3);

		if (firstTime)
		{
			glTextureStorage2D(texture.id, 1, GL_RGB8, texture.width, texture.height);

			firstTime = false;
		}
		
		glTextureSubImage3D(texture.id, 0, 0, 0, i, texture.width, texture.height, 1, GL_RGB, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	glTextureParameteri(texture.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(texture.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(texture.id, GL_TEXTURE_WRAP_S, wrapMode);
	glTextureParameteri(texture.id, GL_TEXTURE_WRAP_T, wrapMode);
	glTextureParameteri(texture.id, GL_TEXTURE_WRAP_R, wrapMode);

	return texture;
}

const void Texture::use(uint32_t textureUnit) const
{
	glBindTextureUnit(textureUnit, id);
}

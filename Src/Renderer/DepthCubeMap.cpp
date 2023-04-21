#include "AriaPCH.h"

#include "DepthCubeMap.h"

#include <glad/glad.h>

DepthCubeMap::DepthCubeMap()
{
}

DepthCubeMap::~DepthCubeMap()
{
}

void DepthCubeMap::create(uint32_t width, uint32_t height)
{
	// DSA
	glCreateFramebuffers(1, &FBO);

	// RBO (Render Buffer Object) for depth buffer.
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &id);

	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTextureParameteri(id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTextureParameterfv(id, GL_TEXTURE_BORDER_COLOR, borderColor);

	glTextureStorage2D(id, 1, GL_DEPTH_COMPONENT24, width, height);

	std::vector<float> imageData(width * height, 1.0f);

	for (auto face = 0; face < 6; face++)
	{
		glTextureSubImage3D(id, 0, 0, 0, face, width, height, 1, GL_DEPTH_COMPONENT, GL_FLOAT, imageData.data());
	}

	glNamedFramebufferTexture(FBO, GL_DEPTH_ATTACHMENT, id, 0);

	glNamedFramebufferDrawBuffer(FBO, GL_NONE);
	glNamedFramebufferReadBuffer(FBO, GL_NONE);

	if (glCheckNamedFramebufferStatus(FBO, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ARIA_CORE_ERROR("framebuffer error\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthCubeMap::bindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void DepthCubeMap::use(uint32_t textureUnit)
{
	glBindTextureUnit(textureUnit, id);
}

void DepthCubeMap::clear(const std::array<float, 4>& clearValue)
{
	bindFBO();
	glClearNamedFramebufferfv(FBO, GL_DEPTH, 0, clearValue.data());
}

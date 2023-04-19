#include "RenderTexture.h"

#include <glad/glad.h>
#include <vector>

#include "Log.h"

RenderTexture::RenderTexture()
{
}

RenderTexture::~RenderTexture()
{
}

void RenderTexture::create(uint32_t width, uint32_t height)
{
	// DSA
	glCreateFramebuffers(1, &FBO);

	// Texture2D for color buffer
	glCreateTextures(GL_TEXTURE_2D, 1, &id);

	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureStorage2D(id, 1, GL_RGB8, width, height);

	std::vector<uint8_t> imageData(width * height * 3);
	glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData.data());

	glNamedFramebufferTexture(FBO, GL_COLOR_ATTACHMENT0, id, 0);

	glNamedFramebufferDrawBuffer(FBO, GL_COLOR_ATTACHMENT0);

	if (glCheckNamedFramebufferStatus(FBO, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ARIA_CORE_ERROR("framebuffer error\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTexture::bindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void RenderTexture::use(uint32_t textureUnit)
{
	glBindTextureUnit(textureUnit, id);
}

void RenderTexture::clear(const std::array<float, 4>& clearColor /*= { 0.4f, 0.6f, 0.9f, 1.0 }*/)
{
	bindFBO();
	glClearNamedFramebufferfv(FBO, GL_COLOR, 0, clearColor.data());
}

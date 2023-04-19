#include "DepthTexture.h"

#include <glad/glad.h>

#include <vector>

#include "Log.h"

DepthTexture::DepthTexture()
{
}

DepthTexture::~DepthTexture()
{
}

void DepthTexture::create(uint32_t width, uint32_t height)
{
	// DSA
	glCreateFramebuffers(1, &FBO);

	// RBO (Render Buffer Object) for depth buffer.
	glCreateTextures(GL_TEXTURE_2D, 1, &id);

	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTextureParameterfv(id, GL_TEXTURE_BORDER_COLOR, borderColor);

	glTextureStorage2D(id, 1, GL_DEPTH_COMPONENT24, width, height);

	std::vector<float> imageData(width * height, 1.0f);
	glTextureSubImage2D(id, 0, 0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, imageData.data());

	glNamedFramebufferTexture(FBO, GL_DEPTH_ATTACHMENT, id, 0);

	glNamedFramebufferDrawBuffer(FBO, GL_NONE);
	glNamedFramebufferReadBuffer(FBO, GL_NONE);

	if (glCheckNamedFramebufferStatus(FBO, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ARIA_CORE_ERROR("Framebuffer error\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthTexture::bindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void DepthTexture::use(uint32_t textureUnit)
{
	glBindTextureUnit(textureUnit, id);
}

void DepthTexture::clear(const std::array<float, 4>& clearValue)
{
	// So the DSA version looks like this:
	bindFBO();
	glClearNamedFramebufferfv(FBO, GL_DEPTH, 0, clearValue.data());
}

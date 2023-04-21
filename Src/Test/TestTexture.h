#pragma once

#include "Test.h"

#include "Renderer/Model.h"
#include "Renderer/Texture.h"
#include "Renderer/Shader.h"

class TestTexture : public Test
{
public:
	TestTexture();
	~TestTexture();

	void onUpdate(float deltaTime) override;
	void onRender(const Shader& shader) override;
	void onImGuiRender() override;
private:
	Mesh mesh;
	Texture texture;
};
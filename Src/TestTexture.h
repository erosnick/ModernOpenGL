#pragma once

#include "Test.h"

#include "Model.h"
#include "Texture.h"
#include <learnopengl/Shader.h>

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
#pragma once

#include "Test.h"

class TestMono : public Test
{
public:
	TestMono();
	~TestMono();

	void onUpdate(float deltaTime) override;
	void onRender(const Shader& shader) override;
	void onImGuiRender() override;
private:
	float clearColor[4];
};
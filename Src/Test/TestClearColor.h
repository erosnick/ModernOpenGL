#pragma once

#include "Test.h"

class TestClearColor : public Test
{
public:
	TestClearColor();
	~TestClearColor();

	void onUpdate(float deltaTime) override;
	void onRender(const Shader& shader) override;
	void onImGuiRender() override;
private:
	float clearColor[4];
};
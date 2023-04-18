#include "TestClearColor.h"

#include <glad/glad.h>

#include <imgui.h>

TestClearColor::TestClearColor()
	: clearColor{ 0.4f, 0.6f, 0.9f, 1.0f }
{
}

TestClearColor::~TestClearColor()
{
}

void TestClearColor::onUpdate(float deltaTime)
{

}

void TestClearColor::onRender(const Shader& shader)
{
	glClearNamedFramebufferfv(0, GL_COLOR, 0, clearColor);

	GLfloat depth[]{ 1.0f, 1.0f, 1.0f, 1.0f };
	glClearNamedFramebufferfv(0, GL_DEPTH, 0, depth);
}

void TestClearColor::onImGuiRender()
{
	ImGui::ColorEdit4("Clear Color:", clearColor);
}

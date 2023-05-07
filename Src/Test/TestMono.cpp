#include "AriaPCH.h"

#include "TestMono.h"

#include <glad/glad.h>

#include <imgui.h>

#include "Scripting/ScriptEngine.h"

TestMono::TestMono()
	: clearColor{ 0.4f, 0.6f, 0.9f, 1.0f }
{
	AriaCore::ScriptEngine::init();
}

TestMono::~TestMono()
{
	AriaCore::ScriptEngine::shutdown();
}

void TestMono::onUpdate(float deltaTime)
{

}

void TestMono::onRender(const Shader& shader)
{
}

void TestMono::onImGuiRender()
{
	if (ImGui::Button("Function Test"))
	{
		AriaCore::ScriptEngine::monoTest();
	}
}

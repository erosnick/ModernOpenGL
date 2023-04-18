#include "Test.h"

#include <imgui.h>

Test::Test()
{
}

Test::~Test()
{
}

TestMenu::TestMenu(Test*& inCurrentTest)
	: currentTest(inCurrentTest)
{

}

TestMenu::~TestMenu()
{

}

void TestMenu::onImGuiRender()
{
	for (auto& test : tests)
	{
		if (ImGui::Button(test.first.c_str()))
		{
			currentTest = test.second();
		}
	}
}

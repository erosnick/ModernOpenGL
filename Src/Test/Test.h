#pragma once

#include <string>
#include <vector>
#include <functional>

#include <learnopengl/Shader.h>

class Test
{
public:
	Test();
	virtual ~Test();

	virtual void onUpdate(float deltaTime) {}
	virtual void onRender(const Shader& shader) {};
	virtual void onImGuiRender() {}
private:

};

class TestMenu : public Test
{
public:
	TestMenu(Test*& inCurrentTest);
	~TestMenu();

	void onImGuiRender() override;
	template<typename T>
	void RegisterTest(const std::string& name)
	{
		ARIA_CLIENT_INFO("Registering test {0}", name);
		tests.push_back(std::make_pair(name, []() {return new T(); }));
	}

private:
	Test*& currentTest;
	std::vector<std::pair<std::string, std::function<Test*()>>> tests;
};
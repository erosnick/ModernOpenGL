// MonoTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "AriaPCH.h"

#include "MonoWrapper.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>

struct Component
{
	int32_t id;
	int32_t tag;
};

uint32_t numComponents = 5;

Component* components = nullptr;

std::unique_ptr<MonoWrapper> monoWrapper;

bool initGLFW();

int32_t CLassLibrary_Component_GetInternalId(const Component* component)
{
	return component->id;
}

int32_t CLassLibrary_Component_get_Tag(MonoObject* thisPtr)
{
	Component* component;
	mono_field_get_value(thisPtr, monoWrapper->getClassField("Component", "handle"), reinterpret_cast<void*>(&component));
	return component->tag;
}

MonoArray* ClassLibrary_Component_GetComponents()
{
	MonoArray* objectArray = monoWrapper->instantiateClassArray("ClassLibrary", "Component", numComponents);

	for (uint32_t i = 0; i < numComponents; i++)
	{
		// Allocate an instance of our class
		MonoObject* object = monoWrapper->instantiateClass("ClassLibrary", "Component");

		void* nativeHandleValue = &components[i];

		monoWrapper->setFieldValue(object, std::string("Component"), std::string("handle"), &nativeHandleValue);
		monoWrapper->setArrayValue<MonoObject*>(objectArray, i, object);
	}

	return objectArray;
}

void ClassLibrary_Game_InitGLFW()
{
	initGLFW();
}

bool initGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	std::cout << "GFLW initialized successfully" << std::endl;

	return true;
}

int main()
{
	monoWrapper = std::make_unique<MonoWrapper>("./ThirdParty/mono/lib", "MonoTest", "Assets/Scripts/ClassLibrary.dll");
	monoWrapper->createClass("ClassLibrary", "Component");

	components = new Component[5];

	for (uint32_t i = 0; i < numComponents; ++i)
	{
		components[i].id = i;
		components[i].tag = i * 4;
	}

	monoWrapper->registerMethod("ClassLibrary.Component::GetInternalId", reinterpret_cast<void*>(CLassLibrary_Component_GetInternalId));
	monoWrapper->registerMethod("ClassLibrary.Component::get_Tag", reinterpret_cast<void*>(CLassLibrary_Component_get_Tag));
	monoWrapper->registerMethod("ClassLibrary.Component::GetComponents", reinterpret_cast<void*>(ClassLibrary_Component_GetComponents));

	monoWrapper->createClass("ClassLibrary", "Main");

	monoWrapper->invokeStaticMethod("Main", "TestComponent");

	monoWrapper->createClass("ClassLibrary", "IGame");
	monoWrapper->createClass("ClassLibrary", "GameMain");

	monoWrapper->registerMethod("ClassLibrary.Game::InitGLFW", reinterpret_cast<void*>(ClassLibrary_Game_InitGLFW));

	monoWrapper->invokeInstanceMethod("IGame", "Init");

	delete[] components;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

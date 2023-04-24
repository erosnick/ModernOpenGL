// MonoTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

struct Component
{
	int32_t id;
	int32_t tag;
};

MonoDomain* monoDomain = nullptr;
MonoClass* componentClass = nullptr;
MonoClassField* nativeHandleField = nullptr;

uint32_t numComponents = 5;

Component* components = nullptr;

int32_t CLassLibrary_Component_GetInternalId(const Component* component)
{
	return component->id;
}

int32_t CLassLibrary_Component_get_Tag(MonoObject* thisPtr)
{
	Component* component;
	mono_field_get_value(thisPtr, nativeHandleField, reinterpret_cast<void*>(&component));
	return component->tag;
}

MonoArray* ClassLibrary_Component_GetComponents()
{
	MonoArray* array = mono_array_new(monoDomain, componentClass, numComponents);

	for (uint32_t i = 0; i < numComponents; i++)
	{
		MonoObject* object = mono_object_new(monoDomain, componentClass);
		mono_runtime_object_init(object);

		void* nativeHandleValue = &components[i];

		mono_field_set_value(object, nativeHandleField, &nativeHandleValue);
		mono_array_set(array, MonoObject*, i, object);
	}

	return array;
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

	return true;
}

int main()
{
	mono_set_dirs("./ThirdParty/mono/lib", "./ThirdParty/mono/lib");

	monoDomain = mono_jit_init("MonoTest");

	MonoAssembly* gameAssembly = nullptr;
	MonoImage* gameImage = nullptr;

	if (monoDomain != nullptr)
	{
		gameAssembly = mono_domain_assembly_open(monoDomain, "ClassLibrary.dll");

		if (gameAssembly != nullptr)
		{
			gameImage = mono_assembly_get_image(gameAssembly);

			if (gameImage != nullptr)
			{
				//MonoClass* testClass = mono_class_from_name(gameImage, "ClassLibrary", "TestClass");

				//MonoMethodDesc* entryPointMethodDesc = mono_method_desc_new("ClassLibrary.TestClass:Call()", true);

				//MonoMethod* entryPointMethod = mono_method_desc_search_in_class(entryPointMethodDesc, testClass);
				//mono_method_desc_free(entryPointMethodDesc);

				//mono_runtime_invoke(entryPointMethod, nullptr, nullptr, nullptr);

				componentClass = mono_class_from_name(gameImage, "ClassLibrary", "Component");

				nativeHandleField = mono_class_get_field_from_name(componentClass, "handle");

				components = new Component[5];

				for (uint32_t i = 0; i < numComponents; ++i)
				{
					components[i].id = i;
					components[i].tag = i * 4;
				}

				mono_add_internal_call("ClassLibrary.Component::GetInternalId", reinterpret_cast<void*>(CLassLibrary_Component_GetInternalId));
				mono_add_internal_call("ClassLibrary.Component::get_Tag", reinterpret_cast<void*>(CLassLibrary_Component_get_Tag));
				mono_add_internal_call("ClassLibrary.Component::GetComponents", reinterpret_cast<void*>(ClassLibrary_Component_GetComponents));
			
				MonoClass* mainClass = mono_class_from_name(gameImage, "ClassLibrary", "Main");

				MonoMethodDesc* entryPointMethodDesc = mono_method_desc_new("ClassLibrary.Main:TestComponent()", true);

				MonoMethod* entryPointMethod = mono_method_desc_search_in_class(entryPointMethodDesc, mainClass);
				mono_method_desc_free(entryPointMethodDesc);

				mono_runtime_invoke(entryPointMethod, nullptr, nullptr, nullptr);
			}
		}
	}

	mono_jit_cleanup(monoDomain);

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

#include "AriaPCH.h"

#include "Core/Core.h"
#include "Core/Log.h"

#include "ScriptingEngine.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

#include <fstream>

namespace AriaCore
{
	struct ScriptingEngineData
	{
		MonoDomain* rootDomain = nullptr;
		MonoDomain* appDomain = nullptr;
	};
	
	static std::unique_ptr<ScriptingEngineData> data;
	
	void ScriptingEngine::init()
	{
		data = std::make_unique<ScriptingEngineData>();

		initMono();
	}
	
	void ScriptingEngine::initMono()
	{
		mono_set_assemblies_path("./ThirdParty/mono/lib");
	
		data->rootDomain = mono_jit_init("AriaJITRuntime");
	
		//ARIA_CORE_ASSERT(data->rootDomain);
	
		if (data->rootDomain != nullptr)
		{
			// Create an App Domain
			data->appDomain = mono_domain_create_appdomain("AriaAppDomain", nullptr);
			mono_domain_set(data->appDomain, true);
		}
	}

	void printAssemblyTypes(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			ARIA_CORE_INFO("{0}.{1}", nameSpace, name);
		}
	}

	char* readBytes(const std::string& filepath, uint32_t* outSize)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
		{
			// Failed to open the file
			return nullptr;
		}

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint32_t size = static_cast<uint32_t>(end - stream.tellg());

		if (size == 0)
		{
			// File is empty
			return nullptr;
		}

		char* buffer = new char[size];
		stream.read((char*)buffer, size);
		stream.close();

		*outSize = size;
		return buffer;
	}

	MonoAssembly* loadCSharpAssembly(const std::string& assemblyPath)
	{
		uint32_t fileSize = 0;
		char* fileData = readBytes(assemblyPath, &fileSize);

		// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

		if (status != MONO_IMAGE_OK)
		{
			const char* errorMessage = mono_image_strerror(status);
			// Log some error message using the errorMessage data
			return nullptr;
		}

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
		mono_image_close(image);

		// Don't forget to free the file data
		delete[] fileData;

		return assembly;
	}
	
	void ScriptingEngine::shutdown()
	{
		mono_jit_cleanup(data->rootDomain);
	}

	void ScriptingEngine::monoTest()
	{
		ARIA_CORE_INFO("Mono test.");

		auto assembly = loadCSharpAssembly("Assets/Scripts/ClassLibrary.dll");

		ARIA_CORE_INFO("printAssemblyTypes");
		printAssemblyTypes(assembly);

		MonoImage* image = mono_assembly_get_image(assembly);

		MonoClass* monoClass = mono_class_from_name(image, "ClassLibrary", "Test");
		ARIA_CORE_ASSERT(monoClass);

		MonoObject* object = mono_object_new(data->appDomain, monoClass);

		// Call the parameterless (default) constructor
		// 如果要调用有参的构造函数，和调用普通函数一样，通过mono_class_get_method_from_name()
		// 进行获取，然后调用
		mono_runtime_object_init(object);

		ARIA_CORE_INFO("Call Test.printMessage()");

		MonoMethod* printMessage = mono_class_get_method_from_name(monoClass, "printMessage", 0);

		mono_runtime_invoke(printMessage, object, nullptr, nullptr);

		ARIA_CORE_INFO("Call Test.printCustomMessageInt(int value)");

		MonoMethod* printCustomMessageInt = mono_class_get_method_from_name(monoClass, "printCustomMessageInt", 1);

		int value = 5;

		void* param = &value;

		mono_runtime_invoke(printCustomMessageInt, object, &param, nullptr);

		ARIA_CORE_INFO("Call Test.printCustomMessageString(string message)");

		MonoMethod* printCustomMessageString = mono_class_get_method_from_name(monoClass, "printCustomMessageString", 1);

		MonoString* message = mono_string_new(data->appDomain, "Hello world from C++.");

		void* stringParam = message;
		mono_runtime_invoke(printCustomMessageString, object, &stringParam, nullptr);
	}
}

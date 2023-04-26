#include "AriaPCH.h"

#include "MonoWrapper.h"

#include <fstream>

#include "Core/Core.h"

MonoDomain* MonoWrapper::monoDomain = nullptr;
MonoDomain* MonoWrapper::monoAppDomain = nullptr;

MonoWrapper::MonoWrapper(const std::string& monoAssemblyDir, const std::string& domainName, const std::string& assemblyPath)
{
	//mono_set_dirs(monoAssemblyDir.c_str(), monoAssemblyDir.c_str());

	mono_set_assemblies_path(monoAssemblyDir.c_str());

	monoDomain = mono_jit_init(domainName.c_str());
	ARIA_CORE_ASSERT(monoDomain);

	// Create an App Domain
	monoAppDomain = mono_domain_create_appdomain("MonoTestAppDomain", nullptr);
	mono_domain_set(monoAppDomain, true);

	//gameAssembly = mono_domain_assembly_open(monoAppDomain, assemblyPath.c_str());

	gameAssembly = loadCSharpAssembly(assemblyPath);

	ARIA_CORE_ASSERT(gameAssembly);

	printAssemblyTypes(gameAssembly);
	//gameImage = mono_assembly_get_image(gameAssembly);
}

MonoWrapper::~MonoWrapper()
{
	mono_jit_cleanup(monoDomain);
	mono_jit_cleanup(monoAppDomain);
}

MonoClass* MonoWrapper::createClass(const std::string& inNameSpace, const std::string& name)
{
	Class classInfo = getClass(name);

	if (classInfo.monoClass == nullptr)
	{
		MonoImage* image = mono_assembly_get_image(gameAssembly);

		MonoClass* monoClass = mono_class_from_name(image, inNameSpace.c_str(), name.c_str());

		Class classInfo;
		classInfo.name = name;
		classInfo.monoClass = monoClass;
		classInfo.nameSpace = inNameSpace;

		classes[name] = classInfo;

		nativeHandleField = mono_class_get_field_from_name(monoClass, "handle");
	}

	return classInfo.monoClass;
}

MonoObject* MonoWrapper::instantiateClass(const char* namespaceName, const char* className)
{
	// Get a reference to the class we want to instantiate
	MonoClass* monoClass = getClass(className).monoClass;

	// Allocate an instance of our class
	// first we pass the AppDomain that we created when we initialized Mono, 
	// remember this isn't the domain we got back from mono_jit_init, it's 
	// AppDomain we explicitly created.
	// Secondly we pass the actual class that we want to allocate an instance of.
	MonoObject* classInstance = mono_object_new(monoAppDomain, monoClass);

	ARIA_CORE_ASSERT(classInstance);

	// Call the parameterless (default) constructor
	mono_runtime_object_init(classInstance);

	return classInstance;
}

MonoArray* MonoWrapper::instantiateClassArray(const char* namespaceName, const char* className, uint32_t count)
{
	// Get a reference to the class we want to instantiate
	MonoClass* monoClass = getClass(className).monoClass;

	MonoArray* classArray = mono_array_new(monoAppDomain, monoClass, count);

	return classArray;
}

void MonoWrapper::registerMethod(const std::string& methodName, void* function)
{
	mono_add_internal_call(methodName.c_str(), function);
}

void MonoWrapper::invokeStaticMethod(const std::string& className, const std::string& name)
{
	Class classInfo = getClass(className);

	std::string fullMethodName = classInfo.getClassName() + ":" + name + "()";

	MonoMethodDesc* entryPointMethodDesc = mono_method_desc_new(fullMethodName.c_str(), true);

	MonoMethod* entryPointMethod = mono_method_desc_search_in_class(entryPointMethodDesc, classInfo.monoClass);
	mono_method_desc_free(entryPointMethodDesc);

	mono_runtime_invoke(entryPointMethod, nullptr, nullptr, nullptr);
}

void MonoWrapper::invokeInstanceMethod(const std::string& className, const std::string& name)
{
	Class gameClassInfo = getClass(className);

	MonoClass* gameMainClass = getClass("GameMain").monoClass;

	MonoMethod* gameMainMethod = findMethod("GameMain", "CreateGame");

	ARIA_CORE_ASSERT(gameMainMethod);

	MonoObject* gameObject = mono_runtime_invoke(gameMainMethod, nullptr, nullptr, nullptr);

	uint32_t gameObjectGCHandle = mono_gchandle_new(gameObject, false);

	std::string fullMethodName = gameClassInfo.getClassName() + ":" + name + "()";

	MonoMethodDesc* fooMethodDesc = mono_method_desc_new(fullMethodName.c_str(), true);
	ARIA_CORE_ASSERT(fooMethodDesc);

	MonoMethod* virtualMethod = mono_method_desc_search_in_class(fooMethodDesc, gameClassInfo.monoClass);
	ARIA_CORE_ASSERT(virtualMethod);

	MonoMethod* fooMethod = mono_object_get_virtual_method(gameObject, virtualMethod);

	mono_runtime_invoke(fooMethod, gameObject, nullptr, nullptr);

	mono_method_desc_free(fooMethodDesc);
}

MonoMethod* MonoWrapper::findMethod(const std::string& className, const std::string& name)
{
	auto findResult = methods.find(name);

	MonoMethod* method = nullptr;

	if (findResult != methods.end())
	{
		method = methods.find(name)->second.method;

		ARIA_CORE_ASSERT(method);
		return method;
	}

	Class classInfo = getClass(className);

	std::string fullMethodName = classInfo.getClassName() + ":" + name + "()";

	MonoMethodDesc* methodDesc = mono_method_desc_new(fullMethodName.c_str(), true);
	ARIA_CORE_ASSERT(methodDesc);

	method = mono_method_desc_search_in_class(methodDesc, classInfo.monoClass);
	ARIA_CORE_ASSERT(method);

	Method methodInfo;
	methodInfo.name = name;
	methodInfo.className = className;
	methodInfo.method = method;
	methods[name] = methodInfo;

	return method;
}

MonoClassField* MonoWrapper::getClassField(const std::string& className, const std::string& name) const
{
	MonoClass* monoClass = classes.at(className).monoClass;
	ARIA_CORE_ASSERT(monoClass);

	MonoClassField* classField = nullptr;

	classField = mono_class_get_field_from_name(monoClass, name.c_str());
	ARIA_CORE_ASSERT(classField);

	return classField;
}

void MonoWrapper::setFieldValue(MonoObject* object, std::string& className, std::string& fieldName, void* value)
{
	mono_field_set_value(object, getClassField(className, fieldName), value);
}

Class MonoWrapper::getClass(const std::string& name) const
{
	auto findResult = classes.find(name);

	if (findResult != classes.end())
	{
		return findResult->second;

	}
	
	return {};
}

void MonoWrapper::printAssemblyTypes(MonoAssembly * assembly)
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

		printf("%s.%s\n", nameSpace, name);
	}
}

MonoAssembly* MonoWrapper::loadCSharpAssembly(const std::string& assemblyPath)
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

char* MonoWrapper::readBytes(const std::string& filepath, uint32_t* outSize)
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

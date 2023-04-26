#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

#include <string>
#include <unordered_map>

struct Class
{
	const std::string getClassName() const
	{
		auto fullName = nameSpace + "." + name;
		return fullName;
	}

	std::string name;
	std::string nameSpace;
	MonoClass* monoClass;
};

struct Method
{
	std::string className;
	std::string name;
	MonoMethod* method;
};

class MonoWrapper
{
public:
	MonoWrapper(const std::string& monoAssemblyDir, const std::string& domainName, const std::string& assemblyPath);
	~MonoWrapper();

	MonoClass* createClass(const std::string& inNameSpace, const std::string& name);

	MonoObject* instantiateClass(const char* namespaceName, const char* className);
	MonoArray* instantiateClassArray(const char* namespaceName, const char* className, uint32_t count);

	void registerMethod(const std::string& methodName, void* function);

	void invokeStaticMethod(const std::string& className, const std::string& name);
	void invokeInstanceMethod(const std::string& className, const std::string& name);

	MonoMethod* findMethod(const std::string& className, const std::string& name);

	static MonoDomain* getMonoDomain() { return monoDomain; }
	static MonoDomain* getMonoAppDomain() { return monoAppDomain; }

	MonoClassField* getClassField(const std::string& className, const std::string& name) const;

	void setFieldValue(MonoObject* object, std::string& className, std::string& fieldName, void* value);

	template<typename T>
	void setArrayValue(MonoArray* objectArray, uint32_t index, MonoObject* object)
	{
		mono_array_set(objectArray, T, index, object);
	}

	Class getClass(const std::string& name) const;

	void printAssemblyTypes(MonoAssembly* assembly);

	MonoAssembly* loadCSharpAssembly(const std::string& assemblyPath);

private:
	char* readBytes(const std::string& filepath, uint32_t* outSize);

private:
	static MonoDomain* monoDomain;
	static MonoDomain* monoAppDomain;
	MonoClassField* nativeHandleField = nullptr;
	MonoAssembly* gameAssembly = nullptr;

	std::unordered_map<std::string, Class> classes;
	std::unordered_map<std::string, Method> methods;
};
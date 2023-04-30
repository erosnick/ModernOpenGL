#include "AriaPCH.h"

#include "ScriptGlue.h"

#include <mono/jit/jit.h>

#include <glm/glm.hpp>

struct Entity
{
	template<typename T>
	void addComponent() {}

	template<typename T>
	bool hasComponent() { return true; }

	template<typename T>
	void removeComponent() {}
};

namespace AriaCore
{
#define ARIA_ADD_INTERNAL_CALL(internalCall) mono_add_internal_call("Aria.InternalCalls::#"#internalCall, static_cast<void*>(InternalCalls::internalCall))

	std::unordered_map<MonoType*, std::function<void(Entity&)>> createComponentFunctions;
	std::unordered_map<MonoType*, std::function<bool(Entity&)>> hasComponentFunctions;
	std::unordered_map<MonoType*, std::function<void(Entity&)>> removeComponentFunctions;

#define ARIA_REGISTER_MANAGED_COMPONENT(type) \
	{\
		MonoType* managedType = mono_reflection_from_name("Aria."#type, ScriptingEngine::getCoreAssemblyInfo()->assemblyImage);\
		if (managedType)\
		{\
			createComponentFunctions[managedType] = [](Entity& entity) { entity.addComponent<type>(); };\
			hasComponentFunctions[managedType] = [](Entity& entity) { return entity.hasComponent<type>(); };\
			removeComponentFunctions[managedType] = [](Entity& entity) { entity.removeComponent<type>(); };\
		}\
		else\
		{\
			ARIA_CORE_ASSERT(false, "No C# component class found for "#type "!");\
		}\
	}

	namespace InternalCalls
	{

	}

	ScriptGlue::ScriptGlue()
	{
	}

	ScriptGlue::~ScriptGlue()
	{
	}

	static void nativeLog(MonoString* message, int value)
	{
		char* messageString = mono_string_to_utf8(message);
		ARIA_CORE_INFO("{0}-{1}", messageString, value);
		mono_free(messageString);
	}

	static void nativeLogVector3(glm::vec3* parameter)
	{
		ARIA_CORE_INFO("x = {0}, y = {1}, z = {2}", parameter->x, parameter->y, parameter->z);
	}

	static void nativeLogVector3WithReturnValue(glm::vec3* parameter, glm::vec3* result)
	{
		ARIA_CORE_INFO("x = {0}, y = {1}, z = {2}", parameter->x, parameter->y, parameter->z);

		*result = glm::cross(*parameter, glm::vec3(0.0f, 0.0f, -1.0f));
	}

	void ScriptGlue::registerFunctions()
	{
		mono_add_internal_call("ClassLibrary.Test::nativeLog", nativeLog);
		mono_add_internal_call("ClassLibrary.Test::nativeLogVector3", nativeLogVector3);
		mono_add_internal_call("ClassLibrary.Test::nativeLogVector3WithReturnValue", nativeLogVector3WithReturnValue);
	}
}
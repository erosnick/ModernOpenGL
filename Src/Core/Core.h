#pragma once

#include <memory>

#define ARIA_EXPAND_MACRO(x) x
#define ARIA_STRINGIFY_MACRO(x) #x

#ifdef ARIA_DEBUG
#define ARIA_CORE_ENABLE_ASSERTS

#if defined(ARIA_PLATFORM_WINDOWS)
	#define ARIA_DEBUGBREAK() __debugbreak()
#endif
#else
	#define ARIA_DEBUGBREAK()
#endif

#ifdef ARIA_CORE_ENABLE_ASSERTS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define ARIA_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { ARIA##type##ERROR(msg, __VA_ARGS__); ARIA_DEBUGBREAK(); } }
	#define ARIA_INTERNAL_ASSERT_WITH_MSG(type, check, ...) ARIA_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define ARIA_INTERNAL_ASSERT_NO_MSG(type, check) ARIA_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", ARIA_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define ARIA_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define ARIA_INTERNAL_ASSERT_GET_MACRO(...) ARIA_EXPAND_MACRO( ARIA_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, ARIA_INTERNAL_ASSERT_WITH_MSG, ARIA_INTERNAL_ASSERT_NO_MSG) )

	#define ARIA_ASSERT(...) ARIA_EXPAND_MACRO( ARIA_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define ARIA_CORE_ASSERT(...) ARIA_EXPAND_MACRO( ARIA_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define ARIA_ASSERT(...)
	#define ARIA_CORE_ASSERT(...)
#endif

#define BIT(x) (1 << x)

#define ARIA_CORE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
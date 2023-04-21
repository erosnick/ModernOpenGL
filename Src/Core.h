#pragma once

#include <memory>

#ifdef ARIA_CORE_DEBUG
#define ARIA_CORE_ENABLE_ASSERTS
#endif

#ifdef ARIA_CORE_ENABLE_ASSERTS
#define ARIA_CORE_ASSERT(x, ...) { if(!(x)) { ARIA_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define ARIA_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define ARIA_CORE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
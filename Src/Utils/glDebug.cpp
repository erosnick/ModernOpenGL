#include "AriaPCH.h"

#include <glad/glad.h>
#include <iostream>

#include "Core.h"

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam) {
	// 忽略一些不重要的错误/警告代码
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	ARIA_CORE_TRACE("---------------");
	ARIA_CORE_ERROR("Debug message ({0}):{1}", id, message);

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             ARIA_CORE_ERROR("Source: API"); break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   ARIA_CORE_ERROR("Source: Window System"); break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: ARIA_CORE_ERROR("Source: Shader Compiler"); break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     ARIA_CORE_ERROR("Source: Third Party"); break;
	case GL_DEBUG_SOURCE_APPLICATION:     ARIA_CORE_ERROR("Source: Application"); break;
	case GL_DEBUG_SOURCE_OTHER:           ARIA_CORE_ERROR("Source: Other"); break;
	}

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               ARIA_CORE_ERROR("Type: Error"); break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: ARIA_CORE_ERROR("Type: Deprecated Behaviour"); break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  ARIA_CORE_ERROR("Type: Undefined Behaviour"); break;
	case GL_DEBUG_TYPE_PORTABILITY:         ARIA_CORE_ERROR("Type: Portability"); break;
	case GL_DEBUG_TYPE_PERFORMANCE:         ARIA_CORE_ERROR("Type: Performance"); break;
	case GL_DEBUG_TYPE_MARKER:              ARIA_CORE_ERROR("Type: Marker"); break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          ARIA_CORE_ERROR("Type: Push Group"); break;
	case GL_DEBUG_TYPE_POP_GROUP:           ARIA_CORE_ERROR("Type: Pop Group"); break;
	case GL_DEBUG_TYPE_OTHER:               ARIA_CORE_ERROR("Type: Other"); break;
	}

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         ARIA_CORE_ERROR("Severity: high"); ARIA_CORE_ASSERT(false, "GL_DEBUG_SEVERITY_HIGH"); break;
	case GL_DEBUG_SEVERITY_MEDIUM:       ARIA_CORE_WARN("Severity: medium"); break;
	case GL_DEBUG_SEVERITY_LOW:          ARIA_CORE_INFO("Severity: low"); break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: ARIA_CORE_TRACE("Severity: notification"); break;
	}
}
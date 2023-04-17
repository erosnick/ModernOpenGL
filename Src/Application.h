#pragma once

#include "OpenGLRenderer.h"

class Application
{
public:
	Application() {}
	~Application() {}

	bool initGLFW();

	bool startup();
	void run();

	// Forward declarations
	static void framebufferSizeCallback(GLFWwindow* inWindow, int32 width, int32 height);
	static void keyCallback(GLFWwindow* inWindow, int32 key, int32 scancode, int32 action, int32 mods);
	static void mouseMoveCallback(GLFWwindow* inWindow, double xpos, double ypos);
	static void mouseScrollCallback(GLFWwindow* inWindow, double xoffset, double yoffset);
	static void mouseButtonCallback(GLFWwindow* inWindow, int32_t button, int32_t action, int32_t mods);

	void processInput(GLFWwindow* inWindow);

	void setupGLFWCallbacks();

	void updateFPSCounter(GLFWwindow* window);
private:
	OpenGLRenderer renderer;
	GLFWwindow* window = nullptr;

	static bool rightMouseButtonDown;
	static bool middleMouseButtonDown;

	static glm::vec2 lastMousePosition;

	static float FrameTime;

	int32 frameCount = 0;

	float frameTime = 0.0f;

	std::vector<glm::ivec2> resolutions{ { 1920, 1080 }, { 2560, 1440 }, { 3840, 2160 } };

	// settings
	const int32 SCREEN_WIDTH = resolutions[0].x;
	const int32 SCREEN_HEIGHT = resolutions[0].y;

	const int32 SHADOW_WIDTH = 2048;
	const int32 SHADOW_HEIGHT = 2048;
};
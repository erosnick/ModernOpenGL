#pragma once

#include "Renderer/OpenGLRenderer.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "Window.h"

namespace AriaCore
{
	class Application
	{
	public:
		Application() {}
		virtual ~Application() {}

		bool initGL();

		bool startup();
		void run();

		// Forward declarations
		static void framebufferSizeCallback(GLFWwindow* inWindow, int32 width, int32 height);

		void processInput();

		void updateFPSCounter(GLFWwindow* window);

		void onEvent(Event& event);

		inline Window& GetWindow() { return *window; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool onWindowClose(WindowCloseEvent& e);

	private:
		static Application* s_Instance;

		OpenGLRenderer renderer;
		GLFWwindow* glfwWindow = nullptr;

		std::unique_ptr<Window> window;

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

		bool running = true;
	};
}
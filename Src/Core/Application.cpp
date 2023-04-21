#include "AriaPCH.h"

#include "Application.h"

#include <GLFW/glfw3.h>

bool Application::rightMouseButtonDown = false;
bool Application::middleMouseButtonDown = false;

glm::vec2 Application::lastMousePosition{ 0.0f, 0.0f };

float Application::FrameTime = 0.0166667f;

bool Application::initGLFW()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	// glfw window creation
	// --------------------
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Colorful Triangle", nullptr, nullptr);

	if (window == nullptr)
	{
		ARIA_CORE_CRITICAL("ailed to create GLFW window.");
		glfwTerminate();
		return false;
	}

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	auto width = mode->width;
	auto height = mode->height;

	glfwSetWindowPos(window, static_cast<int>(width * 0.5f - SCREEN_WIDTH * 0.5f),
		static_cast<int>(height * 0.5f - SCREEN_HEIGHT * 0.5f));

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	setupGLFWCallbacks();

	renderer.setGLFWWindow(window);

	return true;
}

bool Application::startup()
{
	return initGLFW() && renderer.startup();
}

void Application::run()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//Wireframe rendering
	// render loop
	// -----------
	double simulationTime = 0.0;

	while (!glfwWindowShouldClose(window))
	{
		double realTime = glfwGetTime();

		while (simulationTime < realTime)
		{
			simulationTime += 0.016667;

			// input
			// -----
			renderer.update();
			processInput(window);
			glfwPollEvents();
		}

		updateFPSCounter(window);
		renderer.run();
	}

	renderer.imGuiShutdown();

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
}

void Application::framebufferSizeCallback(GLFWwindow* inWindow, int32 width, int32 height)
{
	auto renderer = reinterpret_cast<OpenGLRenderer*>(glfwGetWindowUserPointer(inWindow));

	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	renderer->setViewport(0, 0, width, height);
}

void Application::keyCallback(GLFWwindow* inWindow, int32 key, int32 scancode, int32 action, int32 mods)
{
	auto renderer = reinterpret_cast<OpenGLRenderer*>(glfwGetWindowUserPointer(inWindow));

	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		renderer->toggleTexture();
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		renderer->setRenderMode(ERenderMode::Rasterizing);
	}

	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		renderer->setRenderMode(ERenderMode::RayTracing);
	}

	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		renderer->toggleRenderDepth();
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		renderer->toggleDebugDepth();
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(inWindow, true);
	}

	if (key == GLFW_KEY_KP_1 && action == GLFW_PRESS)
	{
		renderer->scene = ESceneSelection::DirectionalShadow;
	}

	if (key == GLFW_KEY_KP_2 && action == GLFW_PRESS)
	{
		renderer->scene = ESceneSelection::PointShadow;
	}
}

void Application::mouseMoveCallback(GLFWwindow* inWindow, double xpos, double ypos)
{
	auto renderer = reinterpret_cast<OpenGLRenderer*>(glfwGetWindowUserPointer(inWindow));

	auto deltaX = static_cast<float>(xpos - lastMousePosition.x);
	auto deltaY = static_cast<float>(lastMousePosition.y - ypos);

	if (rightMouseButtonDown)
	{
		renderer->getCamera().ProcessMouseMovement(deltaX, deltaY);
	}

	if (middleMouseButtonDown)
	{
		auto app = reinterpret_cast<OpenGLRenderer*>(glfwGetWindowUserPointer(inWindow));
		renderer->getCamera().ProcessKeyboard(LEFT, FrameTime * deltaX);
		renderer->getCamera().ProcessKeyboard(DOWN, FrameTime * deltaY);
	}

	lastMousePosition.x = static_cast<float>(xpos);
	lastMousePosition.y = static_cast<float>(ypos);
}

void Application::mouseScrollCallback(GLFWwindow* inWindow, double xoffset, double yoffset)
{
	auto renderer = reinterpret_cast<OpenGLRenderer*>(glfwGetWindowUserPointer(inWindow));

	renderer->getCamera().ProcessMouseScroll(static_cast<float>(yoffset));
}

void Application::mouseButtonCallback(GLFWwindow* inWindow, int32_t button, int32_t action, int32_t mods)
{
	auto renderer = reinterpret_cast<OpenGLRenderer*>(glfwGetWindowUserPointer(inWindow));

	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS)
	{
		rightMouseButtonDown = true;
	}

	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE)
	{
		rightMouseButtonDown = false;
	}

	if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_PRESS)
	{
		middleMouseButtonDown = true;
	}

	if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_RELEASE)
	{
		middleMouseButtonDown = false;
	}
}

void Application::processInput(GLFWwindow* inWindow)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		renderer.getCamera().ProcessKeyboard(FORWARD, FrameTime);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		renderer.getCamera().ProcessKeyboard(BACKWARD, FrameTime);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		renderer.getCamera().ProcessKeyboard(LEFT, FrameTime);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		renderer.getCamera().ProcessKeyboard(RIGHT, FrameTime);
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		renderer.getCamera().ProcessKeyboard(UP, FrameTime);
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		renderer.getCamera().ProcessKeyboard(DOWN, FrameTime);
	}
}

void Application::setupGLFWCallbacks()
{
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, mouseMoveCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);

	glfwSetWindowUserPointer(window, &renderer);
}

void Application::updateFPSCounter(GLFWwindow* window)
{
	static double previousSeconds = glfwGetTime();
	double currentSeconds = glfwGetTime();
	double elapsedSeconds = currentSeconds - previousSeconds;

	frameTime = static_cast<float>(elapsedSeconds);

	if (elapsedSeconds >= 0.25)
	{
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		auto temp = fmt::format("OpenGL [FPS: {0}]", static_cast<float>(fps));
		glfwSetWindowTitle(window, temp.c_str());
		frameCount = 0;
	}

	frameCount++;
}

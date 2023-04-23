#include "AriaPCH.h"

#include "Application.h"

#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

#include "KeyCodes.h"
#include "MouseButtonCodes.h"

#include "Input.h"

#include <GLFW/glfw3.h>

namespace AriaCore
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	bool Application::onWindowClose(WindowCloseEvent& e)
	{
		running = false;
		return true;
	}

	bool Application::rightMouseButtonDown = false;
	bool Application::middleMouseButtonDown = false;
	
	glm::vec2 Application::lastMousePosition{ 0.0f, 0.0f };
	
	float Application::FrameTime = 0.0166667f;
	
	bool Application::initGL()
	{
		if (!s_Instance)
		{
			// Initialize core
			Log::init();
		}

		ARIA_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		window = std::unique_ptr<Window>(Window::Create({ "Hello Colorful Triangle", static_cast<uint32_t>(SCREEN_WIDTH), static_cast<uint32_t>(SCREEN_HEIGHT) }));
		window->SetEventCallback(BIND_EVENT_FN(onEvent));

		glfwWindow = static_cast<GLFWwindow*>(window->GetNativeWindow());
	
		renderer.setGLFWWindow(glfwWindow);
	
		return true;
	}
	
	bool Application::startup()
	{
		return initGL() && renderer.startup();
	}
	
	void Application::run()
	{
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//Wireframe rendering
		// render loop
		// -----------
		double simulationTime = 0.0;
	
		while (running)
		{
			double realTime = glfwGetTime();
	
			while (simulationTime < realTime)
			{
				simulationTime += 0.016667;
	
				// input
				// -----
				renderer.update();
				processInput();
				window->OnUpdate();
			}
	
			updateFPSCounter(glfwWindow);
			renderer.run();
		}
	
		renderer.imGuiShutdown();
	}
	
	void Application::framebufferSizeCallback(GLFWwindow* inWindow, int32 width, int32 height)
	{
		auto renderer = reinterpret_cast<OpenGLRenderer*>(glfwGetWindowUserPointer(inWindow));
	
		// make sure the viewport matches the new window dimensions; note that width and
		// height will be significantly larger than specified on retina displays.
		renderer->setViewport(0, 0, width, height);
	}
	
	void Application::processInput()
	{
		if (Input::IsKeyPressed(ARIA_KEY_W))
		{
			renderer.getCamera().ProcessKeyboard(FORWARD, FrameTime);
		}

		if (Input::IsKeyPressed(ARIA_KEY_S))
		{
			renderer.getCamera().ProcessKeyboard(BACKWARD, FrameTime);
		}

		if (Input::IsKeyPressed(ARIA_KEY_A))
		{
			renderer.getCamera().ProcessKeyboard(LEFT, FrameTime);
		}
	
		if (Input::IsKeyPressed(ARIA_KEY_D))
		{
			renderer.getCamera().ProcessKeyboard(RIGHT, FrameTime);
		}

		if (Input::IsKeyPressed(ARIA_KEY_Q))
		{
			renderer.getCamera().ProcessKeyboard(UP, FrameTime);
		}
	
		if (Input::IsKeyPressed(ARIA_KEY_E))
		{
			renderer.getCamera().ProcessKeyboard(DOWN, FrameTime);
		}
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

	void Application::onEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseButtonPressedEvent>(
			[&](MouseButtonPressedEvent& e)
			{
				switch (e.GetMouseButton())
				{
				case ARIA_MOUSE_BUTTON_2:
					rightMouseButtonDown = true;
					break;
				case ARIA_MOUSE_BUTTON_3:
					middleMouseButtonDown = true;
					break;
				default:
					break;
				}
				return true;
			});

		dispatcher.Dispatch<MouseButtonReleasedEvent>(
			[&](MouseButtonReleasedEvent& e)
			{
				switch (e.GetMouseButton())
				{
				case ARIA_MOUSE_BUTTON_2:
					rightMouseButtonDown = false;
					break;
				case ARIA_MOUSE_BUTTON_3:
					middleMouseButtonDown = false;
					break;
				default:
					break;
				}
				return true;
			});

		dispatcher.Dispatch<MouseScrolledEvent>(
			[&](MouseScrolledEvent& e)
			{
				renderer.getCamera().ProcessMouseScroll(static_cast<float>(e.GetYOffset()));
				renderer.updateProjectionMatrix();
				return true;
			});

		dispatcher.Dispatch<MouseMovedEvent>(
			[&](MouseMovedEvent& e)
			{
				auto xpos = e.GetX();
				auto ypos = e.GetY();

				auto deltaX = static_cast<float>(xpos - lastMousePosition.x);
				auto deltaY = static_cast<float>(lastMousePosition.y - ypos);

				if (rightMouseButtonDown)
				{
					renderer.getCamera().ProcessMouseMovement(deltaX, deltaY);
				}

				if (middleMouseButtonDown)
				{
					renderer.getCamera().ProcessKeyboard(LEFT, FrameTime * deltaX);
					renderer.getCamera().ProcessKeyboard(DOWN, FrameTime * deltaY);
				}

				lastMousePosition.x = static_cast<float>(xpos);
				lastMousePosition.y = static_cast<float>(ypos);

				return true;
			});

		dispatcher.Dispatch<KeyPressedEvent>(
			[&](KeyPressedEvent& e)
			{
				switch (e.GetKeyCode())
				{
				case ARIA_KEY_ESCAPE:
					running = false;
					break;
				case ARIA_KEY_T:
					renderer.toggleTexture();
					break;
				case ARIA_KEY_1:
					renderer.setRenderMode(ERenderMode::Rasterizing);
					break;
				case ARIA_KEY_2:
					renderer.setRenderMode(ERenderMode::RayTracing);
					break;
				case ARIA_KEY_3:
					renderer.toggleRenderDepth();
					break;
				case ARIA_KEY_SPACE:
					renderer.toggleDebugDepth();
					break;
				case ARIA_KEY_KP_1:
					renderer.scene = ESceneSelection::DirectionalShadow;
					break;
				case ARIA_KEY_KP_2:
					renderer.scene = ESceneSelection::PointShadow;
					break;
				default:
					break;
				}
				return true;
			});
	}
}

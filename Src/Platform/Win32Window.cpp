#include "AriaPCH.h"
#include "Win32Window.h"

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

namespace AriaCore 
{	
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		ARIA_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new Win32Window(props);
	}

	Win32Window::Win32Window(const WindowProps& props)
	{
		Init(props);
	}

	Win32Window::~Win32Window()
	{
		Shutdown();
	}

	void Win32Window::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			ARIA_CORE_ASSERT(success, "Could not intialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);

		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		auto width = mode->width;
		auto height = mode->height;

		glfwSetWindowPos(m_Window, static_cast<int>(width * 0.5f - props.Width * 0.5f), static_cast<int>(height * 0.5f - props.Height * 0.5f));

		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ARIA_CORE_ASSERT(status, "Failed to initialize Glad!");

		ARIA_CORE_INFO("OpenGL Info:");
		ARIA_CORE_INFO("  Vendor: {0}", (char*)glGetString(GL_VENDOR));
		ARIA_CORE_INFO("  Renderer: {0}", (char*)glGetString(GL_RENDERER));
		ARIA_CORE_INFO("  Version: {0}", (char*)glGetString(GL_VERSION));

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(false);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint32_t keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void Win32Window::updateFPSCounter()
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
			glfwSetWindowTitle(m_Window, temp.c_str());
			frameCount = 0;
		}

		frameCount++;
	}

	void Win32Window::Shutdown()
	{
		// glfw: terminate, clearing all previously allocated GLFW resources.
		// ------------------------------------------------------------------
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Win32Window::OnUpdate()
	{
		updateFPSCounter();
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void Win32Window::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool Win32Window::IsVSync() const
	{
		return m_Data.VSync;
	}
}

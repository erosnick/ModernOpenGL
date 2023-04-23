#pragma once

#include "Core/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace AriaCore
{
	class Win32Window : public Window
	{
	public:
		Win32Window(const WindowProps& props);
		virtual ~Win32Window();
	
		void OnUpdate() override;
	
		inline uint32_t GetWidth() const override { return m_Data.Width; }
		inline uint32_t GetHeight() const override { return m_Data.Height; }
	
		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;
	
		inline virtual void* GetNativeWindow() const { return m_Window; }
	private:
		virtual void Init(const WindowProps& props);
		virtual void updateFPSCounter();
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;

		int32_t frameCount = 0;

		float frameTime = 0.0f;
	
		struct WindowData
		{
			std::string Title;
			uint32_t Width, Height;
			bool VSync;
	
			EventCallbackFn EventCallback;
		};
	
		WindowData m_Data;
	};
}
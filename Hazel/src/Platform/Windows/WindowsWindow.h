#pragma once

#include "Hazel/Window.h"

#include <GLFW/glfw3.h>
#include "Hazel/Renderer/OpenGLContext.h"

namespace Hazel {

	class WindowsWindow : public Window//win 系统窗口派生类
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }//设置回调函数
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;
		inline virtual void* GetNativeWindow() const { return m_Window; }
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		OpenGLContext* m_Context;

		struct WindowData//窗口数据：名称、宽、高、是否开启垂直同步、回调函数
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

}


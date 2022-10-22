#pragma once

struct GLFWwindow;

namespace Hazel {

	class OpenGLContext 
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		void Init();
		void SwapBuffers();
	private:
		GLFWwindow* m_WindowHandle;
	};

}
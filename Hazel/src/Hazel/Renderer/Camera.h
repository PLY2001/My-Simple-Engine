#pragma once
#include <GLFW/glfw3.h>

namespace Hazel {
	class Camera
	{
	private:
		glm::vec3 cameraPos;
		glm::vec3 cameraFront;
		glm::vec3 cameraUp;
		float cameraFov;

		

		float lastX;
		float lastY;
		float yaw;
		float pitch;

	public:
		Camera();
		void MouseControl(double xposIn, double yposIn);
		void ScrollControl(double xoffset, double yoffset);
		void KeyControl(GLFWwindow* window, const float deltaTime);
		glm::mat4 SetView();
		glm::mat4 SetProjection(float aspect);
		inline glm::vec3 GetPosition() const { return cameraPos; }
		unsigned int firstMouse;
	};
}
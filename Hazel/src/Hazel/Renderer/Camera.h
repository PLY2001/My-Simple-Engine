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

		

		
		float yaw;
		float pitch;

	public:
		Camera();
		void MouseControl(double xposIn, double yposIn);
		void ScrollControl(double xoffset, double yoffset);
		void KeyControl(GLFWwindow* window, const float deltaTime);
		glm::mat4 SetView();
		glm::mat4 SetProjection(float aspect);
		glm::mat4 SetOrtho(float PictureSize);
		inline glm::vec3 GetPosition() const { return cameraPos; }
		inline glm::vec3 GetFront() const { return cameraFront; }
		inline glm::vec3 GetUp() const { return cameraUp; }
		void ChangePosition(glm::vec3 pos);
		void ChangeFront(glm::vec3 front);
		void ChangeUp(glm::vec3 up);
		unsigned int firstMouse;
		float lastX;
		float lastY;
	};
}
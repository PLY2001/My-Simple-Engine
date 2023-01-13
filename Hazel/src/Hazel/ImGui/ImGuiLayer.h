#pragma once

#include "Hazel/Layer.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"
//#include <map>

namespace Hazel {

	class HAZEL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f; 
		glm::vec3 irb120Pos = glm::vec3(0.0f);
		glm::vec3 irb120Rotate = glm::vec3(0.0f);
		glm::vec3 beltPos = glm::vec3(0.0f);
		glm::vec3 beltRotate = glm::vec3(0.0f);
		bool Save = false;
		bool Loaded = false;

	};

}
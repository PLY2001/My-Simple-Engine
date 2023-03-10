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

		void Loading(std::string info);
	private:
		float m_Time = 0.0f; 
		glm::vec3 Pos = glm::vec3(0.0f);
		glm::vec3 Rotate = glm::vec3(0.0f);
		//glm::vec3 LastRotate = glm::vec3(0.0f);
		glm::vec3 HandPos = glm::vec3(0.0f);
		glm::vec3 HandEular = glm::vec3(0.0f);
		bool Saved = false;
		bool ToSave = false;
		bool Loaded = false;
		bool ToLoad = false;
		float PathTime = 0.5;
		glm::vec3 CircleCenter = glm::vec3(0.0f);
		float TotalTime = 30.0f;
		float AnimaTotalTime = 0.0f;
		int TotalTimeIndex = 0;
		//float TotalAllTime = 10.0;

		int SaveAddress = 0;
		int LoadAddress = 0;

		bool RotateChange = false;
		bool PosChange = false;
		//bool firstRotateChange = true;
		std::vector<const char*> NameList;
		std::vector<int> AmountData;

		glm::vec3 FactoryInMin = glm::vec3(1.0f);
		glm::vec3 FactoryInMax = glm::vec3(1.0f);
		glm::vec3 FactoryProcessMin = glm::vec3(1.0f);
		glm::vec3 FactoryProcessMax = glm::vec3(1.0f);
		glm::vec3 FactoryOutMin = glm::vec3(1.0f);
		glm::vec3 FactoryOutMax = glm::vec3(1.0f);

		int FactoryInAmount = 0;
		int FactoryProcessAmount = 0;
		int FactoryOutAmount = 0;
		int FactoryTotalAmount = 0;

		bool BoxLoaded = false;
		float a = 0.4;
		float b = 1;
	};

}
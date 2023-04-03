#pragma once

#include "Hazel/Layer.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"
#include <Python.h>

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
		float PathTime = 1.0f;
		glm::vec3 CircleCenter = glm::vec3(0.0f);
		float TotalTime = 10.0f;
		float AnimaTotalTime = 0.0f;
		
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
		
		int KeyIndex = 0;
		int TimeObjectIndex = -1;
		int TimeIndex = -1;
		//int AllKeyIndex = 0;
		float size = 1.0f;

		//bool ToPython = false;
		PyObject* pModule = nullptr;
		PyObject* pFunc = nullptr;
	};

}
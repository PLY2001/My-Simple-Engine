#pragma once
#include "Renderer/Model.h"
namespace Hazel {

	class ABBIRB120
	{
	public:
		ABBIRB120(glm::vec3 Pos, glm::vec3 Rotation,std::shared_ptr<Model>& model);
		~ABBIRB120() = default;

		void InitModelMatrices();
		std::vector<std::vector<glm::mat4>> ModelMatrices;//����ģ�͵�model�任��������
		std::vector<std::vector<glm::mat4>> DefaultModelMatrices;//����ģ�͵�model�任��������
		void AddAmount();
		int GetAmount();
		void ChangeAngle();
		float* SetAngle(int Axis);
		
	private:
		glm::vec3 Pos;
		glm::vec3 Rotation;
		std::shared_ptr<Model> model;
		float Angle[6] = { 0,0,0,0,0,0 };
		int Amount = 1;
		
		
	};
}
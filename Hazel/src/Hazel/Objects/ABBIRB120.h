#pragma once
#include "Renderer/Model.h"

#define PI 3.1415926

namespace Hazel {

	class ABBIRB120
	{
	public:
		ABBIRB120(glm::vec3 Pos, glm::vec3 Rotation, glm::vec3 Scale,std::shared_ptr<Model>& model);
		~ABBIRB120() = default;

		void InitModelMatrices();
		std::vector<std::vector<glm::mat4>> ModelMatrices;//生成模型的model变换矩阵数组
		std::vector<std::vector<glm::mat4>> DefaultModelMatrices;//生成模型的model变换矩阵数组
		void AddAmount();
		int GetAmount();
		void ChangeAngle();
		float* SetAngle(int Axis);
		bool SolveAngle(glm::vec3 Pos,glm::vec3 Eular);
		
		std::shared_ptr<Model> model;

		glm::vec3 GetAABBMaxPos(int index);
		glm::vec3 GetAABBMinPos(int index);
		
		glm::vec3 GetScale();
		glm::vec3 GetPos(int index);
		
		void ChangePos(int index,glm::vec3 ChangedPos);

	private:
		std::vector<glm::vec3> m_Pos;
		glm::vec3 Rotation;
		glm::vec3 Scale;
		
		float Angle[6] = { 0,0,0,0,0,0 };
		int Amount = 1;
		enum class ControlMode { Angles = 0, PosEular = 1 };

		//float XMin, XMax, YMin, YMax, ZMin, ZMax = 0;
		std::vector<glm::vec3> AABBMinPos;
		std::vector<glm::vec3> AABBMaxPos;
		void SetAABB();

	public:
		
		ControlMode controlmode = ControlMode::Angles;
	};


	class ModelMatrix
 	{
 	public:
 		glm::mat4 matrix = glm::mat4(1.0f);
 		ModelMatrix(glm::vec3 Pos)
 		{
 			matrix = glm::translate(matrix, Pos);
 		}
 
 	};
}

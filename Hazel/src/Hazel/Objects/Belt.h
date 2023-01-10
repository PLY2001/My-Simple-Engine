#pragma once
#include "Renderer/Model.h"
#include "ModelMatrix.h"


#define PI 3.1415926

namespace Hazel {

	class Belt
	{
	public:
		Belt(glm::vec3 Pos, glm::vec3 Rotation, glm::vec3 Scale, std::shared_ptr<Model>& model);
		~Belt() = default;

		void InitModelMatrices();
		//std::vector<std::vector<glm::mat4>> ModelMatrices;//生成模型的model变换矩阵数组
		std::vector<std::vector<glm::mat4>> DefaultModelMatrices;//生成模型的model变换矩阵数组
		inline glm::mat4 GetDefaltModelMatrices() { return DefaultModelMatrices[0][m_index]; }
		void AddAmount();
		int GetAmount();


		std::shared_ptr<Model> model;

		glm::vec3 GetAABBMaxPos();
		glm::vec3 GetAABBMinPos();

		glm::vec3 GetScale();
		glm::vec3 GetPos();
		glm::vec3 GetRotate();

		void ChangePos(glm::vec3 ChangedPos);
		void ChangeRotate(glm::vec3 ChangedRotate, int RotateAxis);

		void SetChoosedIndex(int index);
		inline int GetChoosedIndex() { return m_index; }

		bool CheckCollision(int index, glm::vec3 CheckPos);

	private:
		std::vector<glm::vec3> m_Pos;
		std::vector<glm::vec3> m_Rotate;
		glm::vec3 Scale;

		std::vector<std::vector<float>> Angle;
		int Amount = 1;
		enum class ControlMode { None = 0, Angles = 1, PosEular = 2 };

		//float XMin, XMax, YMin, YMax, ZMin, ZMax = 0;
		std::vector<glm::vec3> AABBMinPos;
		std::vector<glm::vec3> AABBMaxPos;
		void SetAABB(int index);

		int m_index = -1;

	public:

		ControlMode controlmode = ControlMode::None;
	};



// 	class ModelMatrix
// 	{
// 	public:
// 		glm::mat4 matrix = glm::mat4(1.0f);
// 		ModelMatrix(glm::vec3 Pos)
// 		{
// 			matrix = glm::translate(matrix, Pos);
// 		}
// 
// 	};

}

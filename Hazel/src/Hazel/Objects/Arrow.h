#pragma once
#include "Renderer/Model.h"

namespace Hazel {

	class Arrow
	{
	public:
		Arrow(glm::vec3 Pos, glm::vec3 Rotation, glm::vec3 Scale, std::shared_ptr<Model>& model);
		~Arrow() = default;

		void ChangePos(glm::vec3 ChangedPos);
		void ChangeRotate(glm::qua<float> m_RotateQuaternion);
		glm::mat4 GetModelMatrix();
		std::shared_ptr<Model> model;

		glm::vec3 GetAABBMaxPos(int axis);
		glm::vec3 GetAABBMinPos(int axis);

	private:
		glm::vec3 Pos;
		glm::vec3 Rotation;
		glm::qua<float> RotateQuaternion = glm::qua<float>(glm::vec3(0.0f));
		glm::vec3 Scale;
		glm::mat4 ModelMatrix = glm::mat4(1);//生成模型的model变换矩阵数组

		std::vector<glm::vec3> AABBMinPos;
		std::vector<glm::vec3> AABBMaxPos;
		void SetAABB();
	};

}
#pragma once
#include<numeric>

namespace Hazel {

	struct PathPoint
	{
		glm::vec3 Path_Pos = glm::vec3(0.0f);
		glm::vec3 Path_Rotate = glm::vec3(0.0f);
	};

	class Animation
	{
	public:
		Animation(bool HasAngle);
		~Animation() = default;
		void Reset();

		void SetPathPos(glm::vec3 Pos);
		glm::vec3 GetPathKeyPos(int index);

		void SetPathRotate(glm::vec3 Rotate);
		glm::vec3 GetPathKeyRotate(int index);

		PathPoint GetPathPoint(float TotalTime, float deltaTime);
		PathPoint pathpoint;

		int frame = 0;
		float TimeNow = 0.0f;
		int Path_index = 0;
		bool Playing = false;
	private:
		std::vector<glm::vec3> m_Path_Pos;
		glm::vec3 Path_Pos_Now = glm::vec3(0.0f);
		glm::vec3 Path_Pos_Last = glm::vec3(0.0f);
		std::vector<float> Path_Pos_Mod;

		std::vector<glm::vec3> m_Path_Rotate;
		glm::vec3 Path_Rotate_Now = glm::vec3(0.0f);
		glm::vec3 Path_Rotate_Last = glm::vec3(0.0f);
		std::vector<float> Path_Rotate_Mod;

		std::vector<glm::vec3> m_Path_HandPos;
		std::vector<glm::vec3> m_Path_HandEular;
		bool HaveAngle;
	};

	
}
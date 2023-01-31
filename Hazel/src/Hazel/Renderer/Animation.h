#pragma once
#include<numeric>

namespace Hazel {

	struct PathPoint
	{
		glm::vec3 Path_Pos = glm::vec3(0.0f);
		glm::vec3 Path_Rotate = glm::vec3(0.0f);
		glm::vec3 Path_HandPos = glm::vec3(0.0f);
		glm::vec3 Path_HandEular = glm::vec3(0.0f);
	};

	enum class PathMode { Straght = 0, Circle = 1 };

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

		void SetPathHandPos(glm::vec3 HandPos);
		glm::vec3 GetPathKeyHandPos(int index);

		void SetPathHandEular(glm::vec3 HandEular);
		glm::vec3 GetPathKeyHandEular(int index);

		void SetPathTime(float Time);
		float GetPathTotalTime(int index);
		float GetPathKeyTime(int index);

		void SetPathMode(glm::vec3 CircleCenter);

		void RemovePath();
		int GetPathKeySize();

		int GetPathMode(int index);
		glm::vec3 GetPathCircleCenter(int index);

		PathPoint GetPathPoint(float deltaTime);
		PathPoint pathpoint;

		float TotalTime = 0.0f;
		int frame = 0;
		float TimeNow = 0.0f;
		int Path_index = 0;
		int Key_index = 0;
		bool Playing = false;
		bool HaveAnimation = false;
		PathMode pathmode = PathMode::Straght;
	private:
		std::vector<float> Path_Time;

		std::vector<glm::vec3> m_Path_Pos;
		glm::vec3 Path_Pos_Now = glm::vec3(0.0f);
		glm::vec3 Path_Pos_Last = glm::vec3(0.0f);


		std::vector<glm::vec3> m_Path_Rotate;
		glm::vec3 Path_Rotate_Now = glm::vec3(0.0f);
		glm::vec3 Path_Rotate_Last = glm::vec3(0.0f);


		std::vector<glm::vec3> m_Path_HandPos;
		glm::vec3 Path_HandPos_Now = glm::vec3(0.0f);
		glm::vec3 Path_HandPos_Last = glm::vec3(0.0f);


		std::vector<glm::vec3> m_Path_HandEular;
		glm::vec3 Path_HandEular_Now = glm::vec3(0.0f);
		glm::vec3 Path_HandEular_Last = glm::vec3(0.0f);

		std::vector<PathMode> pathmodelist;
		std::vector<glm::vec3> CircleCenterList;

		bool HaveAngle = false;
	};

	
}
#pragma once
#include<numeric>
#define PI 3.1415926
namespace Hazel {

	struct PathPoint
	{
		glm::vec3 Path_Pos = glm::vec3(0.0f);
		glm::qua<float> Path_Rotate = glm::qua<float>(glm::vec3(0.0f));
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

		void SetPathRotate(glm::qua<float> RotateQuaternion);
		glm::qua<float> GetPathKeyRotate(int index);

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


		std::vector<glm::qua<float>> m_Path_Rotate;
		glm::qua<float> Path_Rotate_Now = glm::qua<float>(glm::vec3(0.0f));
		glm::qua<float> Path_Rotate_Last = glm::qua<float>(glm::vec3(0.0f));


		std::vector<glm::vec3> m_Path_HandPos;
		glm::vec3 Path_HandPos_Now = glm::vec3(0.0f);
		glm::vec3 Path_HandPos_Last = glm::vec3(0.0f);


		std::vector<glm::vec3> m_Path_HandEular;
		glm::vec3 Path_HandEular_Now = glm::vec3(0.0f);
		glm::vec3 Path_HandEular_Last = glm::vec3(0.0f);

		std::vector<PathMode> pathmodelist;
		std::vector<glm::vec3> CircleCenterList;

		bool HaveAngle = false;

		glm::vec3 SolveEularAngle(glm::qua<float> m_RotateQuaternion);
	};

	
}
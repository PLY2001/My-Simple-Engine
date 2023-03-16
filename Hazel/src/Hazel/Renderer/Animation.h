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
		std::string Path_State2;
	};

	enum class PathMode { Straght = 0, Circle = 1 };

	class Animation
	{
	public:
		Animation(bool HasAngle);
		~Animation() = default;
		void Reset();

		void SetPathPos(glm::vec3 Pos);
		void SetPathPos(glm::vec3 Pos,int index);
		void InsertPathPos(glm::vec3 Pos, int index);
		void DeletePathPos(glm::vec3 Pos, int index);
		glm::vec3 GetPathKeyPos(int index);

		void SetPathRotate(glm::qua<float> RotateQuaternion);
		void SetPathRotate(glm::qua<float> RotateQuaternion,int index);
		void InsertPathRotate(glm::qua<float> RotateQuaternion, int index);
		void DeletePathRotate(glm::qua<float> RotateQuaternion, int index);
		glm::qua<float> GetPathKeyRotate(int index);

		void SetPathHandPos(glm::vec3 HandPos);
		void SetPathHandPos(glm::vec3 HandPos,int index);
		void InsertPathHandPos(glm::vec3 HandPos, int index);
		void DeletePathHandPos(glm::vec3 HandPos, int index);
		glm::vec3 GetPathKeyHandPos(int index);

		void SetPathHandEular(glm::vec3 HandEular);
		void SetPathHandEular(glm::vec3 HandEular,int index);
		void InsertPathHandEular(glm::vec3 HandEular, int index);
		void DeletePathHandEular(glm::vec3 HandEular, int index);
		glm::vec3 GetPathKeyHandEular(int index);

		void SetPathTime(float Time);
		void SetPathTime(float Time,int index);
		void InsertPathTime(float Time, int index);
		void DeletePathTime(float Time, int index);
		float GetPathTotalTime(int index);
		float GetPathKeyTime(int index);

		void SetPathMode(glm::vec3 CircleCenter);
		void SetPathMode(glm::vec3 CircleCenter,int index);
		void InsertPathMode(glm::vec3 CircleCenter, int index);
		void DeletePathMode(glm::vec3 CircleCenter, int index);

		void SetPathState2(std::string state2);
		void SetPathState2(std::string state2,int index);
		void InsertPathState2(std::string state2, int index);
		void DeletePathState2(std::string state2, int index);
		std::string GetPathKeyState2(int index);

		void RemovePath();
		int GetPathKeySize();
		bool GetHaveAnimation();

		int GetPathMode(int index);
		glm::vec3 GetPathCircleCenter(int index);

		PathPoint GetPathPoint(float deltaTime);
		PathPoint pathpoint;

		float TotalTime = 0.0f;
		int frame = 0;
		float TimeNow = 0.0f;
		float TotalTimeNow = 0.0f;
		float AllTotalTimeNow = 0.0f;
		float StepTotalTime = 0.0f;
		int Path_index = 0;
		int Key_index = 0;
		bool Playing = false;
		bool StepPlaying = false;
		//bool HaveAnimation = false;
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

		std::vector<std::string> m_Path_State2;

		bool HaveAngle = false;

		glm::vec3 SolveEularAngle(glm::qua<float> m_RotateQuaternion);
	};

	
}
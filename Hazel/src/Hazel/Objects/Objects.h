#pragma once
#include "Renderer/Model.h"
#include "ModelMatrix.h"
#include <map>
#include <fstream>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "Hazel/Renderer/Model.h"
#include "Hazel/Renderer/Animation.h"

#define PI 3.1415926f

namespace Hazel {

	enum class ControlMode { None = 0, Angles = 1, PosEular = 2 };

	struct Object
	{
		std::string m_Name = "NULL";
		std::shared_ptr<Model> m_Model;
		int m_Amount = 0;
		bool m_HaveAngle = false;
		glm::vec3 m_Scale = glm::vec3(1.0f);
		ControlMode m_ControlMode = ControlMode::None;

		std::vector<glm::vec3> m_Pos;
		std::vector<glm::vec3> m_Rotate;
		std::vector<std::vector<float>> m_Angle;
		std::vector<glm::vec3> m_HandPos;
		std::vector<glm::vec3> m_HandEular;
		std::vector<std::vector<glm::mat4>> m_ModelMatrices;//生成模型的model变换矩阵数组
		std::vector<std::vector<glm::mat4>> m_DefaultModelMatrices;//生成模型的model变换矩阵数组
		std::vector<glm::vec3> m_AABBMinPos;
		std::vector<glm::vec3> m_AABBMaxPos;
		std::vector<Animation> m_Anima;
	};

	class Objects
	{
	public:
		std::vector<Object> objects;
		Objects(std::map<std::string, std::shared_ptr<Model>>& modelmap);
		void AddObject(std::string name, glm::vec3 Pos, glm::vec3 Rotation, glm::vec3 Scale, bool hasAngle);
		~Objects() = default;

		//void InitModelMatrices(int ObjectIndex);
		//std::vector<std::vector<std::vector<glm::mat4>>> ModelMatrices;//生成模型的model变换矩阵数组
		//std::vector<std::vector<std::vector<glm::mat4>>> DefaultModelMatrices;//生成模型的model变换矩阵数组
		glm::mat4* GetModelMatrix(int ObjectIndex, int index);
		void AddAmount();
		void AddAmount(std::string name);
		void ReduceAmount();
		void Load_AddAmount();
		int GetAmount(int ObjectIndex);
		int GetMyAmount();
		int GetObjectAmount();
		void ChangeAngle();
		void ChangeAngle(int objectindex, int index);
		float* SetAngle(int Axis);
		//float* SetHandPos(int index);
		//float* SetHandEular(int index);
		bool SolveAngle();
		bool SolveAngle(int objectindex, int index);
		Animation& GetAnimation(int objectindex, int index);
		Animation& GetMyAnimation();
		//std::vector<std::shared_ptr<Model>> m_model;
		std::map<std::string, std::shared_ptr<Model>> m_modelmap;

		glm::vec3 GetAABBMaxPos();
		glm::vec3 GetAABBMinPos();

		glm::vec3 GetScale();
		glm::vec3 GetPos();
		glm::vec3 GetPos(int objectindex, int index);
		glm::vec3 GetRotate();
		glm::vec3 GetRotate(int objectindex, int index);
		glm::vec3 GetHandPos();
		glm::vec3 GetHandEular();


		void ChangePos(glm::vec3 ChangedPos);
		void ChangePos(glm::vec3 ChangedPos, int objectindex, int index);
		void ChangeRotate(glm::vec3 ChangedRotate, int RotateAxis);
		void ChangeRotate(glm::vec3 ChangedRotate, int RotateAxis, int objectindex, int index);
		void ChangeHandPos(glm::vec3 ChangedHandPos);
		void ChangeHandPos(glm::vec3 ChangedHandPos, int objectindex, int index);
		void ChangeHandEular(glm::vec3 ChangedHandEular);
		void ChangeHandEular(glm::vec3 ChangedHandEular, int objectindex, int index);

		void SetChoosedIndex(int ObjectIndex, int index);
		inline int GetChoosedIndex() { return m_index; }

		bool CheckCollision(int ObjectIndex, int index, glm::vec3 CheckPos);

	private:
		//std::vector<std::vector<glm::vec3>> m_Pos;
		//std::vector<std::vector<glm::vec3>> m_Rotate;
		//std::vector<glm::vec3> m_Scale;
		//std::vector<std::vector<glm::vec3>> m_HandPos;
		//std::vector<std::vector<glm::vec3>> m_HandEular;
		
		//std::vector<std::vector<std::vector<float>>> Angle;
		//std::vector<int> Amount;
		int ObjectAmount = 0;
		

		
		//std::vector<std::vector<glm::vec3>> AABBMinPos;
		//std::vector<std::vector<glm::vec3>> AABBMaxPos;
		void SetAABB(int ObjectIndex,int index);

		int m_index = -1;
		int m_Objectindex = -1;
		//std::vector<bool> haveAngle;

	public:

		//std::vector<ControlMode> controlmode;
		ControlMode GetControlMode();
		int* GetControlModeAddress();
		//std::vector<std::string> ObjectsMap;

		bool SaveScene();
		bool LoadScene();

		int load_objectindex = 0;
		std::string load_name;
		int load_amount = 0;
		bool load_haveangle = 0;
		std::string load_filepath;
		glm::vec3 load_scale = glm::vec3(1.0f);

		glm::vec3 load_pos = glm::vec3(0.0f);
		glm::vec3 load_rotate = glm::vec3(0.0f);
		//std::vector<float> load_angle;
	};




}

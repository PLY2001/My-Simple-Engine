#pragma once
#include "Renderer/Model.h"
#include "ModelMatrix.h"
#include <map>
#include <fstream>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "Hazel/Renderer/Model.h"

#define PI 3.1415926

namespace Hazel {

	class Objects
	{
	public:
		
		Objects(std::map<std::string, std::shared_ptr<Model>>& modelmap);
		void AddObject(std::string name, glm::vec3 Pos, glm::vec3 Rotation, glm::vec3 Scale, bool hasAngle);
		~Objects() = default;

		void InitModelMatrices(int ObjectIndex);
		std::vector<std::vector<std::vector<glm::mat4>>> ModelMatrices;//生成模型的model变换矩阵数组
		std::vector<std::vector<std::vector<glm::mat4>>> DefaultModelMatrices;//生成模型的model变换矩阵数组
		glm::mat4* GetModelMatrix(int ObjectIndex, int index);
		void AddAmount();
		void AddAmount(std::string name);
		void ReduceAmount();
		void Load_AddAmount();
		int GetAmount(int ObjectIndex);
		int GetMyAmount();
		int GetObjectAmount();
		void ChangeAngle();
		float* SetAngle(int Axis);
		float* SetPos_Eular(int index);
		bool SolveAngle();

		std::vector<std::shared_ptr<Model>> m_model;
		std::map<std::string, std::shared_ptr<Model>> m_modelmap;

		glm::vec3 GetAABBMaxPos();
		glm::vec3 GetAABBMinPos();

		glm::vec3 GetScale();
		glm::vec3 GetPos();
		glm::vec3 GetRotate();


		void ChangePos(glm::vec3 ChangedPos);
		void ChangeRotate(glm::vec3 ChangedRotate, int RotateAxis);

		void SetChoosedIndex(int ObjectIndex, int index);
		inline int GetChoosedIndex() { return m_index; }

		bool CheckCollision(int ObjectIndex, int index, glm::vec3 CheckPos);

	private:
		std::vector<std::vector<glm::vec3>> m_Pos;
		std::vector<std::vector<glm::vec3>> m_Rotate;
		std::vector<glm::vec3> m_Scale;
		std::vector<std::vector<std::vector<float>>> Pos_Eular;
		//std::map<std::string, glm::vec3[2]> Pos_Eular_map;
		std::vector<std::vector<std::vector<float>>> Angle;
		std::vector<int> Amount;
		int ObjectAmount = 0;
		enum class ControlMode { None = 0, Angles = 1, PosEular = 2 };

		//float XMin, XMax, YMin, YMax, ZMin, ZMax = 0;
		std::vector<std::vector<glm::vec3>> AABBMinPos;
		std::vector<std::vector<glm::vec3>> AABBMaxPos;
		void SetAABB(int ObjectIndex,int index);

		int m_index = -1;
		int m_Objectindex = -1;
		std::vector<bool> haveAngle;

	public:

		std::vector<ControlMode> controlmode;
		ControlMode GetControlMode();
		int* GetControlModeAddress();
		std::vector<std::string> ObjectsMap;

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

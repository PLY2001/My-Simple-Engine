#include "hzpch.h"
#include "Objects.h"

namespace Hazel {

	Objects::Objects(std::map<std::string, std::shared_ptr<Model>>& modelmap)
	{
		m_modelmap = modelmap;
	}

	void Objects::AddObject(std::string name, glm::vec3 Pos, glm::vec3 Rotation, glm::vec3 Scale, bool hasAngle)
	{
		Object object;
		ObjectAmount++;
		object.m_Pos.push_back(Pos);
		object.m_Rotate.push_back(Rotation);
		object.m_RotateQuaternion.push_back(glm::qua<float>(Rotation));
		object.m_Scale = Scale;
		object.m_Model = m_modelmap[name];
		object.m_HaveAngle = hasAngle;
		object.m_Name = name;
		object.m_Amount = 1;

		if (hasAngle)
		{
			object.m_ControlMode = ControlMode::Angles;
		}
		else
		{
			object.m_ControlMode = ControlMode::None;
		}

		
		if (object.m_Model->meshes.size() == 1)
		{
			object.m_Angle.resize(object.m_Model->meshes.size());//[j][k], j是关节索引，k是重复物体索引
			object.m_Angle[0].push_back(0);
		}
		else
		{
			object.m_Angle.resize(object.m_Model->meshes.size() - 1);//[j][k], j是关节索引，k是重复物体索引
			for (int i = 0; i < object.m_Angle.size(); i++)
			{
				object.m_Angle[i].push_back(0);
			}
		}
		
		object.m_HandPos.push_back(glm::vec3(-5.18f, 6.3f, 0.0f));
		object.m_HandEular.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		object.m_HandQuaternion.push_back(glm::qua<float>(glm::vec3(0.0f, 0.0f, 0.0f)));

		//创建变换矩阵
		if (hasAngle)
		{
			object.m_PosScaleMatrices.resize(object.m_Model->meshes.size());//[j][k]，j关节索引，k是重复个体索引
			object.m_ModelMatrices.resize(object.m_Model->meshes.size());
		}
		object.m_DefaultPosScaleMatrices.resize(object.m_Model->meshes.size());
		object.m_RotateMatrices.resize(object.m_Model->meshes.size());
		object.m_DefaultModelMatrices.resize(object.m_Model->meshes.size());
		
		
		for (int i = 0; i < object.m_Model->meshes.size(); i++)
		{
			object.m_RotateMatrices[i].push_back(glm::mat4(1.0f));

			if (hasAngle)
			{
				object.m_PosScaleMatrices[i].push_back(object.m_Model->mModelMatrix);
				object.m_PosScaleMatrices[i].back() = glm::translate(object.m_PosScaleMatrices[i].back(), object.m_Pos[0]);
				object.m_PosScaleMatrices[i].back() = glm::scale(object.m_PosScaleMatrices[i].back(), object.m_Scale);

				object.m_ModelMatrices[i].push_back(object.m_PosScaleMatrices[i].back()* object.m_RotateMatrices[i].back());
			}
			object.m_DefaultPosScaleMatrices[i].push_back(object.m_Model->mModelMatrix);
			object.m_DefaultPosScaleMatrices[i].back() = glm::translate(object.m_DefaultPosScaleMatrices[i].back(), object.m_Pos[0]);
			object.m_DefaultPosScaleMatrices[i].back() = glm::scale(object.m_DefaultPosScaleMatrices[i].back(), object.m_Scale);

			

			object.m_DefaultModelMatrices[i].push_back(object.m_DefaultPosScaleMatrices[i].back() * object.m_RotateMatrices[i].back());
			
		}


		object.m_AABBMinPos.push_back(object.m_Pos[0]);
		object.m_AABBMaxPos.push_back(object.m_Pos[0]);

		Animation Anima(hasAngle);
		object.m_Anima.push_back(Anima);

		objects.push_back(object);

		SetAABB(ObjectAmount-1, 0);

		m_index = 0;
		m_Objectindex = ObjectAmount - 1;
		
	}




	glm::mat4* Objects::GetModelMatrix(int ObjectIndex, int index)
	{
		if (objects[ObjectIndex].m_HaveAngle)
		{
			return objects[ObjectIndex].m_ModelMatrices[index].data();
		}
		else
		{
			return objects[ObjectIndex].m_DefaultModelMatrices[index].data();
		}
	}

	void Objects::AddAmount()
	{
		
		//增加模型
		objects[m_Objectindex].m_Amount++;
		//m_index = objects[m_Objectindex].m_Amount - 2;
		if (m_index < 0)
		{
			m_index = 0;
		}
		if (int increase = objects[m_Objectindex].m_Amount - objects[m_Objectindex].m_DefaultPosScaleMatrices[0].size() > 0)
		{
			for (int j = 0; j < increase; j++)
			{
				objects[m_Objectindex].m_Pos.push_back(objects[m_Objectindex].m_Pos[m_index]);
				objects[m_Objectindex].m_Rotate.push_back(objects[m_Objectindex].m_Rotate[m_index]);
				objects[m_Objectindex].m_RotateQuaternion.push_back(objects[m_Objectindex].m_RotateQuaternion[m_index]);
			}
			for (int i = 0; i < objects[m_Objectindex].m_Model->meshes.size(); i++)
			{
				for (int j = 0; j < increase; j++)
				{
					objects[m_Objectindex].m_RotateMatrices[i].push_back(objects[m_Objectindex].m_RotateMatrices[i].back());

					if (objects[m_Objectindex].m_HaveAngle)
					{
						objects[m_Objectindex].m_PosScaleMatrices[i].push_back(ModelMatrix(glm::vec3(objects[m_Objectindex].m_Pos.back())).matrix);
						//objects[m_Objectindex].m_ModelMatrices[i].back() = glm::rotate(objects[m_Objectindex].m_ModelMatrices[i].back(), objects[m_Objectindex].m_Rotate.back().y, glm::vec3(0.0f,1.0f,0.0f));
						objects[m_Objectindex].m_PosScaleMatrices[i].back() = glm::scale(objects[m_Objectindex].m_PosScaleMatrices[i].back(), objects[m_Objectindex].m_Scale);

						objects[m_Objectindex].m_ModelMatrices[i].push_back(objects[m_Objectindex].m_PosScaleMatrices[i].back()* objects[m_Objectindex].m_RotateMatrices[i].back());
					}
					objects[m_Objectindex].m_DefaultPosScaleMatrices[i].push_back(ModelMatrix(glm::vec3(objects[m_Objectindex].m_Pos.back())).matrix);
					//objects[m_Objectindex].m_DefaultModelMatrices[i].back() = glm::rotate(objects[m_Objectindex].m_DefaultModelMatrices[i].back(), objects[m_Objectindex].m_Rotate.back().y, glm::vec3(0.0f, 1.0f, 0.0f));//需修改为四元数
					objects[m_Objectindex].m_DefaultPosScaleMatrices[i].back() = glm::scale(objects[m_Objectindex].m_DefaultPosScaleMatrices[i].back(), objects[m_Objectindex].m_Scale);

					

					objects[m_Objectindex].m_DefaultModelMatrices[i].push_back(objects[m_Objectindex].m_DefaultPosScaleMatrices[i].back() * objects[m_Objectindex].m_RotateMatrices[i].back());
					
				}
			}

		}
		objects[m_Objectindex].m_AABBMinPos.push_back(objects[m_Objectindex].m_Pos[objects[m_Objectindex].m_Amount - 1]);
		objects[m_Objectindex].m_AABBMaxPos.push_back(objects[m_Objectindex].m_Pos[objects[m_Objectindex].m_Amount - 1]);
		
		for (int i = 0; i < objects[m_Objectindex].m_Angle.size(); i++)
		{
			objects[m_Objectindex].m_Angle[i].push_back(0);
		}
		objects[m_Objectindex].m_HandPos.push_back(glm::vec3(-5.18f, 6.3f, 0.0f));
		objects[m_Objectindex].m_HandEular.push_back(glm::vec3(0.0f,0.0f, 0.0f));
		objects[m_Objectindex].m_HandQuaternion.push_back(glm::qua<float>(glm::vec3(0.0f)));
		
		SetAABB(m_Objectindex, objects[m_Objectindex].m_Amount - 1);

		Animation Anima(objects[m_Objectindex].m_HaveAngle);
		objects[m_Objectindex].m_Anima.push_back(Anima);

		m_index = objects[m_Objectindex].m_Amount - 1;
	}

	void Objects::AddAmount(std::string name)
	{
		int i = 0;
		for (Object object : objects)
		{
			if (object.m_Name == name)
			{
				m_Objectindex = i;
				break;
			}
			i++;
		}
		
		AddAmount();
	}

	void Objects::AddAmount(glm::vec3 Pos, glm::vec3 Rotate)
	{
		//增加模型
		objects[m_Objectindex].m_Amount++;
		m_index = objects[m_Objectindex].m_Amount - 2;
		if (m_index < 0)
		{
			m_index = 0;
		}
		if (int increase = objects[m_Objectindex].m_Amount - objects[m_Objectindex].m_DefaultPosScaleMatrices[0].size() > 0)
		{
			for (int j = 0; j < increase; j++)
			{
				objects[m_Objectindex].m_Pos.push_back(Pos);
				objects[m_Objectindex].m_Rotate.push_back(Rotate);
				objects[m_Objectindex].m_RotateQuaternion.push_back(glm::qua<float>(Rotate));
			}
			for (int i = 0; i < objects[m_Objectindex].m_Model->meshes.size(); i++)
			{
				for (int j = 0; j < increase; j++)
				{
					glm::mat4 RotateMatrix = glm::mat4(1.0f);
					RotateMatrix = glm::mat4_cast(objects[m_Objectindex].m_RotateQuaternion.back()) * RotateMatrix;
					objects[m_Objectindex].m_RotateMatrices[i].push_back(RotateMatrix);

					if (objects[m_Objectindex].m_HaveAngle)
					{
						objects[m_Objectindex].m_PosScaleMatrices[i].push_back(ModelMatrix(glm::vec3(objects[m_Objectindex].m_Pos.back())).matrix);
						//objects[m_Objectindex].m_ModelMatrices[i].back() = glm::rotate(objects[m_Objectindex].m_ModelMatrices[i].back(), objects[m_Objectindex].m_Rotate.back().y, glm::vec3(0.0f, 1.0f, 0.0f));
						objects[m_Objectindex].m_PosScaleMatrices[i].back() = glm::scale(objects[m_Objectindex].m_PosScaleMatrices[i].back(), objects[m_Objectindex].m_Scale);

						objects[m_Objectindex].m_ModelMatrices[i].push_back(objects[m_Objectindex].m_PosScaleMatrices[i].back()* objects[m_Objectindex].m_RotateMatrices[i].back());
					}
					objects[m_Objectindex].m_DefaultPosScaleMatrices[i].push_back(ModelMatrix(glm::vec3(objects[m_Objectindex].m_Pos.back())).matrix);
					//objects[m_Objectindex].m_DefaultModelMatrices[i].back() = glm::rotate(objects[m_Objectindex].m_DefaultModelMatrices[i].back(), objects[m_Objectindex].m_Rotate.back().y, glm::vec3(0.0f, 1.0f, 0.0f));
					objects[m_Objectindex].m_DefaultPosScaleMatrices[i].back() = glm::scale(objects[m_Objectindex].m_DefaultPosScaleMatrices[i].back(), objects[m_Objectindex].m_Scale);

					

					objects[m_Objectindex].m_DefaultModelMatrices[i].push_back(objects[m_Objectindex].m_DefaultPosScaleMatrices[i].back() * objects[m_Objectindex].m_RotateMatrices[i].back());
				}
			}

		}
		objects[m_Objectindex].m_AABBMinPos.push_back(objects[m_Objectindex].m_Pos[objects[m_Objectindex].m_Amount - 1]);
		objects[m_Objectindex].m_AABBMaxPos.push_back(objects[m_Objectindex].m_Pos[objects[m_Objectindex].m_Amount - 1]);

		for (int i = 0; i < objects[m_Objectindex].m_Angle.size(); i++)
		{
			objects[m_Objectindex].m_Angle[i].push_back(0);
		}
		objects[m_Objectindex].m_HandPos.push_back(glm::vec3(-5.18f, 6.3f, 0.0f));
		objects[m_Objectindex].m_HandEular.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		objects[m_Objectindex].m_HandQuaternion.push_back(glm::qua<float>(glm::vec3(0.0f)));

		SetAABB(m_Objectindex, objects[m_Objectindex].m_Amount - 1);

		Animation Anima(objects[m_Objectindex].m_HaveAngle);
		objects[m_Objectindex].m_Anima.push_back(Anima);

		m_index = objects[m_Objectindex].m_Amount - 1;
	}

	void Objects::ReduceAmount()
	{
		objects[m_Objectindex].m_Amount--;
		
		if (m_index < 0)
		{
			m_index = objects[m_Objectindex].m_Amount-1;
		}
		if (int decrease = objects[m_Objectindex].m_DefaultPosScaleMatrices[0].size() - objects[m_Objectindex].m_Amount > 0)
		{
			for (int j = 0; j < decrease; j++)
			{
				objects[m_Objectindex].m_Pos.erase(objects[m_Objectindex].m_Pos.begin() + m_index);
				objects[m_Objectindex].m_Rotate.erase(objects[m_Objectindex].m_Rotate.begin() + m_index);
				objects[m_Objectindex].m_RotateQuaternion.erase(objects[m_Objectindex].m_RotateQuaternion.begin() + m_index);
			}
			for (int i = 0; i < objects[m_Objectindex].m_Model->meshes.size(); i++)
			{
				for (int j = 0; j < decrease; j++)
				{
					if (objects[m_Objectindex].m_HaveAngle)
					{
						objects[m_Objectindex].m_PosScaleMatrices[i].erase(objects[m_Objectindex].m_PosScaleMatrices[i].begin() + m_index);

						objects[m_Objectindex].m_ModelMatrices[i].erase(objects[m_Objectindex].m_ModelMatrices[i].begin() + m_index);
					}
					objects[m_Objectindex].m_DefaultPosScaleMatrices[i].erase(objects[m_Objectindex].m_DefaultPosScaleMatrices[i].begin() + m_index);

					objects[m_Objectindex].m_RotateMatrices[i].erase(objects[m_Objectindex].m_RotateMatrices[i].begin() + m_index);

					objects[m_Objectindex].m_DefaultModelMatrices[i].erase(objects[m_Objectindex].m_DefaultModelMatrices[i].begin() + m_index);
				}
			}

		}
		objects[m_Objectindex].m_AABBMinPos.erase(objects[m_Objectindex].m_AABBMinPos.begin() + m_index);
		objects[m_Objectindex].m_AABBMaxPos.erase(objects[m_Objectindex].m_AABBMaxPos.begin() + m_index);

		for (int i = 0; i < objects[m_Objectindex].m_Angle.size(); i++)
		{
			objects[m_Objectindex].m_Angle[i].erase(objects[m_Objectindex].m_Angle[i].begin() + m_index);
		}
		objects[m_Objectindex].m_HandPos.erase(objects[m_Objectindex].m_HandPos.begin() + m_index);
		objects[m_Objectindex].m_HandEular.erase(objects[m_Objectindex].m_HandEular.begin() + m_index);
		objects[m_Objectindex].m_HandQuaternion.erase(objects[m_Objectindex].m_HandQuaternion.begin() + m_index);

		objects[m_Objectindex].m_Anima.erase(objects[m_Objectindex].m_Anima.begin() + m_index);

		m_index = -1;
		
		
		
			
		
		
	}

	void Objects::ReduceObjectAmount()
	{
		objects.erase(objects.begin() + m_Objectindex);
		ObjectAmount--;
		m_Objectindex = -1;
		m_index = -1;
	}

	void Objects::Load_AddAmount()
	{
		
		//增加模型
		objects[m_Objectindex].m_Amount++;
		//m_index = Amount[m_Objectindex] - 1;
		if (int increase = objects[m_Objectindex].m_Amount - objects[m_Objectindex].m_DefaultPosScaleMatrices[0].size() > 0)
		{
			for (int j = 0; j < increase; j++)
			{
				objects[m_Objectindex].m_Pos.push_back(glm::vec3(0.0f));
				objects[m_Objectindex].m_Rotate.push_back(glm::vec3(0.0f));
				objects[m_Objectindex].m_RotateQuaternion.push_back(glm::qua<float>(glm::vec3(0.0f)));
			}
			for (int i = 0; i < objects[m_Objectindex].m_Model->meshes.size(); i++)
			{
				for (int j = 0; j < increase; j++)
				{
					glm::mat4 RotateMatrix = glm::mat4(1.0f);
					RotateMatrix = glm::mat4_cast(objects[m_Objectindex].m_RotateQuaternion.back()) * RotateMatrix;
					objects[m_Objectindex].m_RotateMatrices[i].push_back(RotateMatrix);
					if (objects[m_Objectindex].m_HaveAngle)
					{
						objects[m_Objectindex].m_PosScaleMatrices[i].push_back(ModelMatrix(glm::vec3(objects[m_Objectindex].m_Pos.back())).matrix);
						//objects[m_Objectindex].m_ModelMatrices[i].back() = glm::rotate(objects[m_Objectindex].m_ModelMatrices[i].back(), objects[m_Objectindex].m_Rotate.back().y, glm::vec3(0.0f, 1.0f, 0.0f));
						objects[m_Objectindex].m_PosScaleMatrices[i].back() = glm::scale(objects[m_Objectindex].m_PosScaleMatrices[i].back(), objects[m_Objectindex].m_Scale);

						objects[m_Objectindex].m_ModelMatrices[i].push_back(objects[m_Objectindex].m_PosScaleMatrices[i].back()* objects[m_Objectindex].m_RotateMatrices[i].back());
					}
					objects[m_Objectindex].m_DefaultPosScaleMatrices[i].push_back(ModelMatrix(glm::vec3(objects[m_Objectindex].m_Pos.back())).matrix);
					//objects[m_Objectindex].m_DefaultModelMatrices[i].back() = glm::rotate(objects[m_Objectindex].m_DefaultModelMatrices[i].back(), objects[m_Objectindex].m_Rotate.back().y, glm::vec3(0.0f, 1.0f, 0.0f));
					objects[m_Objectindex].m_DefaultPosScaleMatrices[i].back() = glm::scale(objects[m_Objectindex].m_DefaultPosScaleMatrices[i].back(), objects[m_Objectindex].m_Scale);

					

					objects[m_Objectindex].m_DefaultModelMatrices[i].push_back(objects[m_Objectindex].m_DefaultPosScaleMatrices[i].back() * objects[m_Objectindex].m_RotateMatrices[i].back());
				}
			}

		}
		objects[m_Objectindex].m_AABBMinPos.push_back(objects[m_Objectindex].m_Pos[objects[m_Objectindex].m_Amount - 1]);
		objects[m_Objectindex].m_AABBMaxPos.push_back(objects[m_Objectindex].m_Pos[objects[m_Objectindex].m_Amount - 1]);

		for (int i = 0; i < objects[m_Objectindex].m_Angle.size(); i++)
		{
			objects[m_Objectindex].m_Angle[i].push_back(0);
		}
		objects[m_Objectindex].m_HandPos.push_back(glm::vec3( -5.18f, 6.3f, 0.0f));
		objects[m_Objectindex].m_HandEular.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		objects[m_Objectindex].m_HandQuaternion.push_back(glm::qua<float>(glm::vec3(0.0f)));

		Animation Anima(objects[m_Objectindex].m_HaveAngle);
		objects[m_Objectindex].m_Anima.push_back(Anima);

		SetAABB(m_Objectindex, objects[m_Objectindex].m_Amount - 1);
	}

	int Objects::GetAmount(int ObjectIndex)
	{
		if (ObjectIndex > -1)
		{
			return objects[ObjectIndex].m_Amount;
		}
		else
		{
			return 0;
		}
	}

	int Objects::GetMyAmount()
	{
		if (m_Objectindex > -1)
		{
			return objects[m_Objectindex].m_Amount;
		}
		else
		{
			return 0;
		}
	}

	int Objects::GetObjectAmount()
	{
		return ObjectAmount;
	}

	void Objects::ChangeAngle()
	{
		//归位
		for (int j = 0; j < objects[m_Objectindex].m_Model->meshes.size(); j++)
		{
			// 		for (int i = 0; i < Amount; i++)
			// 		{
			

			objects[m_Objectindex].m_ModelMatrices[j][m_index] = objects[m_Objectindex].m_DefaultModelMatrices[j][m_index];
			// 		}

		}


		for (int j = 1; j < objects[m_Objectindex].m_Model->meshes.size(); j++)
		{
			// 		for (int i = 0; i < Amount; i++)
			// 		{
			if (j > 0)
			{
				objects[m_Objectindex].m_ModelMatrices[j][m_index] = glm::rotate(objects[m_Objectindex].m_ModelMatrices[j][m_index], glm::radians(objects[m_Objectindex].m_Angle[0][m_index]), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (j > 1)
			{

				objects[m_Objectindex].m_ModelMatrices[j][m_index] = glm::translate(objects[m_Objectindex].m_ModelMatrices[j][m_index], glm::vec3(0.0f, 290.0f, 0.0f));
				objects[m_Objectindex].m_ModelMatrices[j][m_index] = glm::rotate(objects[m_Objectindex].m_ModelMatrices[j][m_index], glm::radians(objects[m_Objectindex].m_Angle[1][m_index]), glm::vec3(0.0f, 0.0f, 1.0f));
				objects[m_Objectindex].m_ModelMatrices[j][m_index] = glm::translate(objects[m_Objectindex].m_ModelMatrices[j][m_index], glm::vec3(0.0f, -290.0f, 0.0f));

			}
			if (j > 2)
			{
				objects[m_Objectindex].m_ModelMatrices[j][m_index] = glm::translate(objects[m_Objectindex].m_ModelMatrices[j][m_index], glm::vec3(0.0f, 560.0f, 0.0f));
				objects[m_Objectindex].m_ModelMatrices[j][m_index] = glm::rotate(objects[m_Objectindex].m_ModelMatrices[j][m_index], glm::radians(objects[m_Objectindex].m_Angle[2][m_index]), glm::vec3(0.0f, 0.0f, 1.0f));
				objects[m_Objectindex].m_ModelMatrices[j][m_index] = glm::translate(objects[m_Objectindex].m_ModelMatrices[j][m_index], glm::vec3(0.0f, -560.0f, 0.0f));
			}
			if (j > 3)
			{
				objects[m_Objectindex].m_ModelMatrices[j][m_index] = glm::translate(objects[m_Objectindex].m_ModelMatrices[j][m_index], glm::vec3(0.0f, 630.0f, 0.0f));
				objects[m_Objectindex].m_ModelMatrices[j][m_index] = glm::rotate(objects[m_Objectindex].m_ModelMatrices[j][m_index], glm::radians(objects[m_Objectindex].m_Angle[3][m_index]), glm::vec3(1.0f, 0.0f, 0.0f));
				objects[m_Objectindex].m_ModelMatrices[j][m_index] = glm::translate(objects[m_Objectindex].m_ModelMatrices[j][m_index], glm::vec3(0.0f, -630.0f, 0.0f));
			}
			if (j > 4)
			{
				objects[m_Objectindex].m_ModelMatrices[j][m_index] = glm::translate(objects[m_Objectindex].m_ModelMatrices[j][m_index], glm::vec3(-302.0f, 630.0f, 0.0f));
				objects[m_Objectindex].m_ModelMatrices[j][m_index] = glm::rotate(objects[m_Objectindex].m_ModelMatrices[j][m_index], glm::radians(objects[m_Objectindex].m_Angle[4][m_index]), glm::vec3(0.0f, 0.0f, 1.0f));
				objects[m_Objectindex].m_ModelMatrices[j][m_index] = glm::translate(objects[m_Objectindex].m_ModelMatrices[j][m_index], glm::vec3(302.0f, -630.0f, 0.0f));
			}
			if (j > 5)
			{
				objects[m_Objectindex].m_ModelMatrices[j][m_index] = glm::translate(objects[m_Objectindex].m_ModelMatrices[j][m_index], glm::vec3(-302.0f, 630.0f, 0.0f));
				objects[m_Objectindex].m_ModelMatrices[j][m_index] = glm::rotate(objects[m_Objectindex].m_ModelMatrices[j][m_index], glm::radians(objects[m_Objectindex].m_Angle[5][m_index]), glm::vec3(1.0f, 0.0f, 0.0f));
				objects[m_Objectindex].m_ModelMatrices[j][m_index] = glm::translate(objects[m_Objectindex].m_ModelMatrices[j][m_index], glm::vec3(302.0f, -630.0f, 0.0f));
			}

			//}

		}
		SetAABB(m_Objectindex,m_index);

	}

	void Objects::ChangeAngle(int objectindex, int index)
	{
		//归位
		for (int j = 0; j < objects[objectindex].m_Model->meshes.size(); j++)
		{
			// 		for (int i = 0; i < Amount; i++)
			// 		{
			
			objects[objectindex].m_ModelMatrices[j][index] = objects[objectindex].m_DefaultModelMatrices[j][index];
			// 		}

		}


		for (int j = 1; j < objects[objectindex].m_Model->meshes.size(); j++)
		{
			// 		for (int i = 0; i < Amount; i++)
			// 		{
			if (j > 0)
			{
				objects[objectindex].m_ModelMatrices[j][index] = glm::rotate(objects[objectindex].m_ModelMatrices[j][index], glm::radians(objects[objectindex].m_Angle[0][index]), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (j > 1)
			{

				objects[objectindex].m_ModelMatrices[j][index] = glm::translate(objects[objectindex].m_ModelMatrices[j][index], glm::vec3(0.0f, 290.0f, 0.0f));
				objects[objectindex].m_ModelMatrices[j][index] = glm::rotate(objects[objectindex].m_ModelMatrices[j][index], glm::radians(objects[objectindex].m_Angle[1][index]), glm::vec3(0.0f, 0.0f, 1.0f));
				objects[objectindex].m_ModelMatrices[j][index] = glm::translate(objects[objectindex].m_ModelMatrices[j][index], glm::vec3(0.0f, -290.0f, 0.0f));

			}
			if (j > 2)
			{
				objects[objectindex].m_ModelMatrices[j][index] = glm::translate(objects[objectindex].m_ModelMatrices[j][index], glm::vec3(0.0f, 560.0f, 0.0f));
				objects[objectindex].m_ModelMatrices[j][index] = glm::rotate(objects[objectindex].m_ModelMatrices[j][index], glm::radians(objects[objectindex].m_Angle[2][index]), glm::vec3(0.0f, 0.0f, 1.0f));
				objects[objectindex].m_ModelMatrices[j][index] = glm::translate(objects[objectindex].m_ModelMatrices[j][index], glm::vec3(0.0f, -560.0f, 0.0f));
			}
			if (j > 3)
			{
				objects[objectindex].m_ModelMatrices[j][index] = glm::translate(objects[objectindex].m_ModelMatrices[j][index], glm::vec3(0.0f, 630.0f, 0.0f));
				objects[objectindex].m_ModelMatrices[j][index] = glm::rotate(objects[objectindex].m_ModelMatrices[j][index], glm::radians(objects[objectindex].m_Angle[3][index]), glm::vec3(1.0f, 0.0f, 0.0f));
				objects[objectindex].m_ModelMatrices[j][index] = glm::translate(objects[objectindex].m_ModelMatrices[j][index], glm::vec3(0.0f, -630.0f, 0.0f));
			}
			if (j > 4)
			{
				objects[objectindex].m_ModelMatrices[j][index] = glm::translate(objects[objectindex].m_ModelMatrices[j][index], glm::vec3(-302.0f, 630.0f, 0.0f));
				objects[objectindex].m_ModelMatrices[j][index] = glm::rotate(objects[objectindex].m_ModelMatrices[j][index], glm::radians(objects[objectindex].m_Angle[4][index]), glm::vec3(0.0f, 0.0f, 1.0f));
				objects[objectindex].m_ModelMatrices[j][index] = glm::translate(objects[objectindex].m_ModelMatrices[j][index], glm::vec3(302.0f, -630.0f, 0.0f));
			}
			if (j > 5)
			{
				objects[objectindex].m_ModelMatrices[j][index] = glm::translate(objects[objectindex].m_ModelMatrices[j][index], glm::vec3(-302.0f, 630.0f, 0.0f));
				objects[objectindex].m_ModelMatrices[j][index] = glm::rotate(objects[objectindex].m_ModelMatrices[j][index], glm::radians(objects[objectindex].m_Angle[5][index]), glm::vec3(1.0f, 0.0f, 0.0f));
				objects[objectindex].m_ModelMatrices[j][index] = glm::translate(objects[objectindex].m_ModelMatrices[j][index], glm::vec3(302.0f, -630.0f, 0.0f));
			}

			//}

		}
		SetAABB(objectindex, index);
	}

	float* Objects::SetAngle(int Axis)
	{
		return &objects[m_Objectindex].m_Angle[Axis - 1][m_index];
	}

// 	float* Objects::SetHandPos(int index)
// 	{
// 		return &objects[m_Objectindex].m_HandPos[m_index][index];
// 	}
// 
// 	float* Objects::SetHandEular(int index)
// 	{
// 		return &objects[m_Objectindex].m_HandEular[m_index][index];
// 	}

	bool Objects::SolveAngle()
	{
		
		glm::vec3 Pos = objects[m_Objectindex].m_HandPos[m_index]/ objects[m_Objectindex].m_Scale;
		glm::vec3 Eular = objects[m_Objectindex].m_HandEular[m_index];
		
		glm::mat4 backwardTranslate = glm::mat4(1);
		backwardTranslate = glm::translate(backwardTranslate, glm::vec3(216.0f, 0.0f, 0.0f));//夹具中心到上一轴的距离为72+144


		glm::mat4 backwardEular = glm::mat4(1);
		backwardEular = glm::translate(backwardEular, Pos);
		glm::qua<float> Quaternion = glm::qua<float>(glm::radians(Eular));
		objects[m_Objectindex].m_HandQuaternion[m_index] = Quaternion;
		glm::mat4 RotateMatrix = glm::mat4(1.0f);
		RotateMatrix = glm::mat4_cast(Quaternion) * RotateMatrix;
		backwardEular = backwardEular * RotateMatrix;
// 		backwardEular = glm::rotate(backwardEular, glm::radians(Eular.x), glm::vec3(1, 0, 0));
// 		backwardEular = glm::rotate(backwardEular, glm::radians(Eular.y), glm::vec3(0, 1, 0));
// 		backwardEular = glm::rotate(backwardEular, glm::radians(Eular.z), glm::vec3(0, 0, 1));
		backwardEular = glm::translate(backwardEular, -Pos);

		glm::vec4 Pw = backwardEular * backwardTranslate * glm::vec4(Pos, 1.0f);

		// 	ImGui::Text("PwX = %.3f", Pw.x);
		// 	ImGui::Text("PwY = %.3f", Pw.y);
		// 	ImGui::Text("PwZ = %.3f", Pw.z);

		float SolvedAngles[6] = { 0,0,0,0,0,0 };
		SolvedAngles[0] = atan2f(Pw.z, -Pw.x) / PI * 180.0f;
		//ImGui::Text("Angle1 = %.3f", SolvedAngles[0]);

		float x = sqrt(Pw.x * Pw.x + Pw.z * Pw.z);
		float y = Pw.y - 290.0f;
		float temp = (float)acos((y * y + x * x - 169004.0f) / 167403.484f);
		SolvedAngles[2] = (temp + (float)atan2f(70.0f, 302.0f)) / PI * 180.0f - 90.0f;
		SolvedAngles[1] = atan2f((-y * 310.006f * sin(temp) + x * (270.0f + 310.006f * cos(temp))), (x * 310.006f * sin(temp) + y * (270.0f + 310.006f * cos(temp)))) * 180.0f / PI;
		//	ImGui::Text("Angle2 = %.3f", SolvedAngles[1]);
		// 	ImGui::Text("Angle3 = %.3f", SolvedAngles[2]);



		glm::mat4 T13 = glm::mat4(1);
		T13 = glm::translate(T13, glm::vec3(-302.0f, 630.0f, 0.0f));
//  		glm::qua<float> Quaternion1 = glm::qua<float>(glm::radians(glm::vec3(0.0f, SolvedAngles[0], SolvedAngles[1]+ SolvedAngles[2])));
//  		glm::mat4 RotateMatrix1 = glm::mat4(1.0f);
//  		RotateMatrix1 = glm::mat4_cast(Quaternion1) * RotateMatrix1;
//  		T13 =  T13*RotateMatrix1;
		T13 = glm::rotate(T13, glm::radians(SolvedAngles[0]), glm::vec3(0.0f, 1.0f, 0.0f));
		T13 = glm::rotate(T13, glm::radians(SolvedAngles[1]), glm::vec3(0.0f, 0.0f, 1.0f));
		T13 = glm::rotate(T13, glm::radians(SolvedAngles[2]), glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 T16 = glm::mat4(1);
		T16 = glm::translate(T16, Pos);
		glm::qua<float> Quaternion2 = glm::qua<float>(glm::radians(Eular));
		glm::mat4 RotateMatrix2 = glm::mat4(1.0f);
		RotateMatrix2 = glm::mat4_cast(Quaternion2) * RotateMatrix2;
		T16 = T16 * RotateMatrix2;
//  		T16 = glm::rotate(T16, glm::radians(Eular.z), glm::vec3(0, 0, 1));
//  		T16 = glm::rotate(T16, glm::radians(Eular.y), glm::vec3(0, 1, 0));
//  		T16 = glm::rotate(T16, glm::radians(Eular.x), glm::vec3(1, 0, 0));

		glm::mat4 T35 = glm::inverse(T13) * T16;
		
		SolvedAngles[3] = atan2f(T35[0].z, T35[0].y) / PI * 180.0f;//[0]表示第一列，z表示第三行
		//SolvedAngles[3] = 0.0f;
		SolvedAngles[5] = atan2f(T35[2].x, -T35[1].x) / PI * 180.0f;
		//SolvedAngles[5] = 0.0f;
		SolvedAngles[4] = atan2f(-T35[1].x / cos(SolvedAngles[5] * PI / 180.0f), T35[0].x) / PI * 180.0f;
		//SolvedAngles[4] = 0.0f;
		
		// 	ImGui::Text("Angle4 = %.3f", SolvedAngles[3]);
		// 	ImGui::Text("Angle5 = %.3f", SolvedAngles[4]);


		if (SolvedAngles[0] < 165.0f && SolvedAngles[0] > -165.0f)
		{
			objects[m_Objectindex].m_Angle[0][m_index] = SolvedAngles[0];
		}
		else
		{
			return false;
		}
		if (SolvedAngles[1] < 110.0f && SolvedAngles[1] > -110.0f)
		{
			objects[m_Objectindex].m_Angle[1][m_index] = SolvedAngles[1];
		}
		else
		{
			return false;
		}
		if (SolvedAngles[2] < 70.0f && SolvedAngles[2] > -110.0f)
		{
			objects[m_Objectindex].m_Angle[2][m_index] = SolvedAngles[2];
		}
		else
		{
			return false;
		}

		//if (SolvedAngles[3] < 160.0f && SolvedAngles[3] > -160.0f)
		objects[m_Objectindex].m_Angle[3][m_index] = SolvedAngles[3];

		if (SolvedAngles[4] < 120.0f && SolvedAngles[4] > -120.0f)
		{
		objects[m_Objectindex].m_Angle[4][m_index] = SolvedAngles[4];
	}
		else
		{
		return false;
		}
		if (SolvedAngles[5] < 400.0f && SolvedAngles[5] > -400.0f)
		{
			objects[m_Objectindex].m_Angle[5][m_index] = SolvedAngles[5];
		}
		else
		{
			return false;
		}
		//ChangeAngle();
		return true;

	}

	bool Objects::SolveAngle(int objectindex, int index)
	{
		glm::vec3 Pos = objects[objectindex].m_HandPos[index] / objects[objectindex].m_Scale;
		glm::vec3 Eular = objects[objectindex].m_HandEular[index];

		glm::mat4 backwardTranslate = glm::mat4(1);
		backwardTranslate = glm::translate(backwardTranslate, glm::vec3(216.0f, 0.0f, 0.0f));//夹具中心到上一轴的距离为72+144


		glm::mat4 backwardEular = glm::mat4(1);
		backwardEular = glm::translate(backwardEular, Pos);
		glm::qua<float> Quaternion = glm::qua<float>(glm::radians(Eular));
		objects[objectindex].m_HandQuaternion[index] = Quaternion;
		glm::mat4 RotateMatrix = glm::mat4(1.0f);
		RotateMatrix = glm::mat4_cast(Quaternion) * RotateMatrix;
		backwardEular = backwardEular * RotateMatrix;
//  		backwardEular = glm::rotate(backwardEular, glm::radians(Eular.x), glm::vec3(1, 0, 0));
//  		backwardEular = glm::rotate(backwardEular, glm::radians(Eular.y), glm::vec3(0, 1, 0));
//  		backwardEular = glm::rotate(backwardEular, glm::radians(Eular.z), glm::vec3(0, 0, 1));
		backwardEular = glm::translate(backwardEular, -Pos);

		glm::vec4 Pw = backwardEular * backwardTranslate * glm::vec4(Pos, 1.0f);

		// 	ImGui::Text("PwX = %.3f", Pw.x);
		// 	ImGui::Text("PwY = %.3f", Pw.y);
		// 	ImGui::Text("PwZ = %.3f", Pw.z);

		float SolvedAngles[6] = { 0,0,0,0,0,0 };
		SolvedAngles[0] = atan2f(Pw.z, -Pw.x) / PI * 180.0f;
		//ImGui::Text("Angle1 = %.3f", SolvedAngles[0]);

		float x = sqrt(Pw.x * Pw.x + Pw.z * Pw.z);
		float y = Pw.y - 290.0f;
		float temp = (float)acos((y * y + x * x - 169004.0f) / 167403.484f);
		SolvedAngles[2] = (temp + (float)atan2f(70.0f, 302.0f)) / PI * 180.0f - 90.0f;
		SolvedAngles[1] = atan2f((-y * 310.006f * sin(temp) + x * (270.0f + 310.006f * cos(temp))), (x * 310.006f * sin(temp) + y * (270.0f + 310.006f * cos(temp)))) * 180.0f / PI;
		//	ImGui::Text("Angle2 = %.3f", SolvedAngles[1]);
		// 	ImGui::Text("Angle3 = %.3f", SolvedAngles[2]);



		glm::mat4 T13 = glm::mat4(1);
		T13 = glm::translate(T13, glm::vec3(-302.0f, 630.0f, 0.0f));
// 		glm::qua<float> Quaternion1 = glm::qua<float>(glm::radians(glm::vec3(0.0f, SolvedAngles[0], SolvedAngles[1] + SolvedAngles[2])));
// 		glm::mat4 RotateMatrix1 = glm::mat4(1.0f);
// 		RotateMatrix1 = glm::mat4_cast(Quaternion1) * RotateMatrix1;
// 		T13 = T13 * RotateMatrix1;
 		T13 = glm::rotate(T13, glm::radians(SolvedAngles[0]), glm::vec3(0.0f, 1.0f, 0.0f));
 		//T13 = glm::translate(T13, glm::vec3(0.0f, 290.0f, 0.0f));
 		T13 = glm::rotate(T13, glm::radians(SolvedAngles[1]), glm::vec3(0.0f, 0.0f, 1.0f));
 		//T13 = glm::translate(T13, glm::vec3(0.0f, -290.0f, 0.0f));
 		//T13 = glm::translate(T13, glm::vec3(0.0f, 560.0f, 0.0f));
 		T13 = glm::rotate(T13, glm::radians(SolvedAngles[2]), glm::vec3(0.0f, 0.0f, 1.0f));
 		//T13 = glm::translate(T13, glm::vec3(0.0f, -560.0f, 0.0f));

		glm::mat4 T16 = glm::mat4(1);
		T16 = glm::translate(T16, Pos);
		glm::qua<float> Quaternion2 = glm::qua<float>(glm::radians(Eular));
		glm::mat4 RotateMatrix2 = glm::mat4(1.0f);
		RotateMatrix2 = glm::mat4_cast(Quaternion2) * RotateMatrix2;
		T16 = T16 * RotateMatrix2;
//  		T16 = glm::rotate(T16, glm::radians(Eular.z), glm::vec3(0, 0, 1));
//  		T16 = glm::rotate(T16, glm::radians(Eular.y), glm::vec3(0, 1, 0));
//  		T16 = glm::rotate(T16, glm::radians(Eular.x), glm::vec3(1, 0, 0));

		glm::mat4 T35 = glm::inverse(T13) * T16;

		SolvedAngles[3] = atan2f(T35[0].z, T35[0].y) / PI * 180.0f;//[0]表示第一列，z表示第三行
		//SolvedAngles[3] = 0.0f;
		SolvedAngles[5] = atan2f(T35[2].x, -T35[1].x) / PI * 180.0f;
		//SolvedAngles[5] = 0.0f;
		SolvedAngles[4] = atan2f(-T35[1].x / cos(SolvedAngles[5] * PI / 180.0f), T35[0].x) / PI * 180.0f;
		//SolvedAngles[4] = 0.0f;

		// 	ImGui::Text("Angle4 = %.3f", SolvedAngles[3]);
		// 	ImGui::Text("Angle5 = %.3f", SolvedAngles[4]);


		if (SolvedAngles[0] < 165.0f && SolvedAngles[0] > -165.0f)
		{
			objects[objectindex].m_Angle[0][index] = SolvedAngles[0];
		}
		else
		{
			return false;
		}
		if (SolvedAngles[1] < 110.0f && SolvedAngles[1] > -110.0f)
		{
			objects[objectindex].m_Angle[1][index] = SolvedAngles[1];
		}
		else
		{
			return false;
		}
		if (SolvedAngles[2] < 70.0f && SolvedAngles[2] > -110.0f)
		{
			objects[objectindex].m_Angle[2][index] = SolvedAngles[2];
		}
		else
		{
			return false;
		}

		//if (SolvedAngles[3] < 160.0f && SolvedAngles[3] > -160.0f)
		objects[objectindex].m_Angle[3][index] = SolvedAngles[3];

		if (SolvedAngles[4] < 120.0f && SolvedAngles[4] > -120.0f)
		{
			objects[objectindex].m_Angle[4][index] = SolvedAngles[4];
		}
		else
		{
			return false;
		}
		if (SolvedAngles[5] < 400.0f && SolvedAngles[5] > -400.0f)
		{
			objects[objectindex].m_Angle[5][index] = SolvedAngles[5];
		}
		else
		{
			return false;
		}
		//ChangeAngle();
		return true;
	}

	Hazel::Animation& Objects::GetAnimation(int objectindex, int index)
	{
		
		return objects[objectindex].m_Anima[index];
		
	}

	Hazel::Animation& Objects::GetMyAnimation()
	{
		if (m_Objectindex > -1 && m_index > -1)
		{
			return objects[m_Objectindex].m_Anima[m_index];
		}
		else
		{
			return Animation(false);
		}
	}

	glm::vec3 Objects::GetAABBMaxPos()
	{
		if (m_Objectindex > -1 && m_index > -1)
		{
			return objects[m_Objectindex].m_AABBMaxPos[m_index];
		}
		else
		{
			return glm::vec3(0.0f);
		}
	}

	glm::vec3 Objects::GetAABBMinPos()
	{
		if (m_Objectindex > -1 && m_index > -1)
		{
			return objects[m_Objectindex].m_AABBMinPos[m_index];
		}
		else
		{
			return glm::vec3(0.0f);
		}
	}



	glm::vec3 Objects::GetScale()
	{
		if (m_Objectindex > -1)
		{
			return objects[m_Objectindex].m_Scale;
		}
		else
		{
			return glm::vec3(1.0f);
		}
	}

	glm::vec3 Objects::GetPos()
	{
		if (m_Objectindex > -1 && m_index > -1)
		{
			return objects[m_Objectindex].m_Pos[m_index];
		}
		else
		{
			return glm::vec3(0.0f);
		}
	}


	glm::vec3 Objects::GetPos(int objectindex, int index)
	{
		return objects[objectindex].m_Pos[index];
	}

	glm::vec3 Objects::GetRotate()
	{
		if (m_Objectindex > -1 && m_index > -1)
		{
			return objects[m_Objectindex].m_Rotate[m_index];
			//return glm::eulerAngles(objects[m_Objectindex].m_RotateQuaternion[m_index]);
		}
		else
		{
			return glm::vec3(0.0f);
		}
	}

	glm::vec3 Objects::GetRotate(int objectindex, int index)
	{
		return objects[objectindex].m_Rotate[index];
		//return glm::eulerAngles(objects[objectindex].m_RotateQuaternion[index]);
	}

	glm::qua<float> Objects::GetRotateQuaternion()
	{
		if (m_Objectindex > -1 && m_index > -1)
		{
			return objects[m_Objectindex].m_RotateQuaternion[m_index];
		}
		else
		{
			return glm::qua<float>(glm::vec3(0.0f));
		}
	}

	glm::vec3 Objects::GetHandPos()
	{
		return objects[m_Objectindex].m_HandPos[m_index];
	}

	glm::vec3 Objects::GetHandEular()
	{
		return objects[m_Objectindex].m_HandEular[m_index];
		//return glm::eulerAngles(objects[m_Objectindex].m_HandQuaternion[m_index]);
	}

// 	glm::qua<float> Objects::GetHandQuaternion()
// 	{
// 		return glm::eulerAngles(objects[m_Objectindex].m_HandQuaternion[m_index]);
// 	}

	void Objects::ChangePos(glm::vec3 ChangedPos)
	{
		objects[m_Objectindex].m_Pos[m_index] += ChangedPos;
		for (int i = 0; i < objects[m_Objectindex].m_Model->meshes.size(); i++)
		{
			//ModelMatrices[i][index] = glm::translate(ModelMatrices[i][index],ChangedPos/Scale);
// 			glm::qua<float> Quaternion = glm::qua<float>(-objects[m_Objectindex].m_Rotate[m_index]);
// 			glm::mat4 RotateMatrix = glm::mat4(1.0f);
// 			RotateMatrix = glm::mat4_cast(Quaternion) * RotateMatrix;
// 			glm::qua<float> Quaternion1 = glm::qua<float>(objects[m_Objectindex].m_Rotate[m_index]);
// 			glm::mat4 RotateMatrix1 = glm::mat4(1.0f);
// 			RotateMatrix1 = glm::mat4_cast(Quaternion1) * RotateMatrix1;

			//objects[m_Objectindex].m_DefaultModelMatrices[i][m_index] =  objects[m_Objectindex].m_DefaultModelMatrices[i][m_index]* RotateMatrix;
			objects[m_Objectindex].m_DefaultPosScaleMatrices[i][m_index] = glm::translate(objects[m_Objectindex].m_DefaultPosScaleMatrices[i][m_index], ChangedPos / objects[m_Objectindex].m_Scale);
			//objects[m_Objectindex].m_DefaultModelMatrices[i][m_index] = objects[m_Objectindex].m_DefaultModelMatrices[i][m_index]*RotateMatrix1;

			objects[m_Objectindex].m_DefaultModelMatrices[i][m_index] = objects[m_Objectindex].m_DefaultPosScaleMatrices[i][m_index] * objects[m_Objectindex].m_RotateMatrices[i][m_index];
		}
		if(objects[m_Objectindex].m_HaveAngle)
		{
			ChangeAngle();
		}
		SetAABB(m_Objectindex,m_index);
	}

	void Objects::ChangePos(glm::vec3 ChangedPos, int objectindex, int index)
	{
		objects[objectindex].m_Pos[index] += ChangedPos;
		for (int i = 0; i < objects[objectindex].m_Model->meshes.size(); i++)
		{
			//ModelMatrices[i][index] = glm::translate(ModelMatrices[i][index],ChangedPos/Scale);
// 			glm::qua<float> Quaternion = glm::qua<float>(-objects[objectindex].m_Rotate[index]);
// 			glm::mat4 RotateMatrix = glm::mat4(1.0f);
// 			RotateMatrix = glm::mat4_cast(Quaternion) * RotateMatrix;
// 			glm::qua<float> Quaternion1 = glm::qua<float>(objects[objectindex].m_Rotate[index]);
// 			glm::mat4 RotateMatrix1 = glm::mat4(1.0f);
// 			RotateMatrix1 = glm::mat4_cast(Quaternion1) * RotateMatrix1;

			//objects[objectindex].m_DefaultModelMatrices[i][index] =  objects[objectindex].m_DefaultModelMatrices[i][index]*RotateMatrix;
			objects[objectindex].m_DefaultPosScaleMatrices[i][index] = glm::translate(objects[objectindex].m_DefaultPosScaleMatrices[i][index], ChangedPos / objects[objectindex].m_Scale);
			//objects[objectindex].m_DefaultModelMatrices[i][index] = objects[objectindex].m_DefaultModelMatrices[i][index]* RotateMatrix1;
			objects[objectindex].m_DefaultModelMatrices[i][index] = objects[objectindex].m_DefaultPosScaleMatrices[i][index] * objects[objectindex].m_RotateMatrices[i][index];
		}
		if (objects[objectindex].m_HaveAngle)
		{
			ChangeAngle(objectindex,index);
		}
		SetAABB(objectindex, index);
	}

	void Objects::ChangeRotate(glm::vec3 ChangedRotate)
	{

		
		//m_ChangedRotate = ChangedRotate;
		objects[m_Objectindex].m_RotateQuaternion[m_index] = objects[m_Objectindex].m_RotateQuaternion[m_index]*glm::qua<float>(ChangedRotate - objects[m_Objectindex].m_Rotate[m_index]);
		
		for (int i = 0; i < objects[m_Objectindex].m_Model->meshes.size(); i++)
		{
			
			//glm::qua<float> Quaternion = glm::qua<float>(ChangedRotate - objects[m_Objectindex].m_Rotate[m_index]);
			glm::mat4 RotateMatrix = glm::mat4(1.0f);
			RotateMatrix = glm::mat4_cast(glm::qua<float>(ChangedRotate - objects[m_Objectindex].m_Rotate[m_index])) * RotateMatrix;
			//objects[m_Objectindex].m_DefaultModelMatrices[i][m_index] = glm::rotate(objects[m_Objectindex].m_DefaultModelMatrices[i][m_index], ChangedRotate[RotateAxis], RotateAxisVec3);
			//objects[m_Objectindex].m_DefaultModelMatrices[i][m_index] = glm::translate(objects[m_Objectindex].m_DefaultModelMatrices[i][m_index], -objects[m_Objectindex].m_Pos[m_index] / objects[m_Objectindex].m_Scale);

			//objects[m_Objectindex].m_DefaultModelMatrices[i][m_index] =  objects[m_Objectindex].m_DefaultModelMatrices[i][m_index]*RotateMatrix;
			objects[m_Objectindex].m_RotateMatrices[i][m_index] = objects[m_Objectindex].m_RotateMatrices[i][m_index]*RotateMatrix;

			//objects[m_Objectindex].m_DefaultModelMatrices[i][m_index] = glm::translate(objects[m_Objectindex].m_DefaultModelMatrices[i][m_index], objects[m_Objectindex].m_Pos[m_index] / objects[m_Objectindex].m_Scale);

			objects[m_Objectindex].m_DefaultModelMatrices[i][m_index] = objects[m_Objectindex].m_DefaultPosScaleMatrices[i][m_index] * objects[m_Objectindex].m_RotateMatrices[i][m_index];
		}
		objects[m_Objectindex].m_Rotate[m_index] = ChangedRotate;
		if (objects[m_Objectindex].m_HaveAngle)
		{
			ChangeAngle();
		}
		SetAABB(m_Objectindex,m_index);
	}

	void Objects::ChangeRotate(glm::vec3 ChangedRotate, int objectindex, int index)
	{
		
		objects[objectindex].m_RotateQuaternion[index] = objects[objectindex].m_RotateQuaternion[index]*glm::qua<float>(ChangedRotate - objects[objectindex].m_Rotate[index]);
		
		for (int i = 0; i < objects[objectindex].m_Model->meshes.size(); i++)
		{
			//glm::qua<float> Quaternion = glm::qua<float>(ChangedRotate);
			glm::mat4 RotateMatrix = glm::mat4(1.0f);
			RotateMatrix = glm::mat4_cast(glm::qua<float>(ChangedRotate - objects[objectindex].m_Rotate[index])) * RotateMatrix;
			//objects[objectindex].m_DefaultModelMatrices[i][index] = glm::translate(objects[objectindex].m_DefaultModelMatrices[i][index], -objects[objectindex].m_Pos[index] / objects[objectindex].m_Scale);

			//objects[objectindex].m_DefaultModelMatrices[i][index] = objects[objectindex].m_DefaultModelMatrices[i][index]* RotateMatrix;
			objects[objectindex].m_RotateMatrices[i][index] = objects[objectindex].m_RotateMatrices[i][index]*RotateMatrix;

			//objects[objectindex].m_DefaultModelMatrices[i][index] = glm::translate(objects[objectindex].m_DefaultModelMatrices[i][index], objects[objectindex].m_Pos[index] / objects[objectindex].m_Scale);

			objects[objectindex].m_DefaultModelMatrices[i][index] = objects[objectindex].m_DefaultPosScaleMatrices[i][index] * objects[objectindex].m_RotateMatrices[i][index];
		}
		objects[objectindex].m_Rotate[index] = ChangedRotate;
		if (objects[objectindex].m_HaveAngle)
		{
			ChangeAngle(objectindex, index);
		}
		SetAABB(objectindex, index);
	}

	void Objects::ChangeRotateD(glm::vec3 ChangedRotate, int objectindex, int index)
	{
		objects[objectindex].m_Rotate[index] = ChangedRotate;
		objects[objectindex].m_RotateQuaternion[index] = glm::qua<float>(ChangedRotate);

		for (int i = 0; i < objects[objectindex].m_Model->meshes.size(); i++)
		{
			//glm::qua<float> Quaternion = glm::qua<float>(ChangedRotate);
			glm::mat4 RotateMatrix = glm::mat4(1.0f);
			RotateMatrix = glm::mat4_cast(objects[objectindex].m_RotateQuaternion[index]) * RotateMatrix;
			//objects[objectindex].m_DefaultModelMatrices[i][index] = glm::translate(objects[objectindex].m_DefaultModelMatrices[i][index], -objects[objectindex].m_Pos[index] / objects[objectindex].m_Scale);

			//objects[objectindex].m_DefaultModelMatrices[i][index] = objects[objectindex].m_DefaultModelMatrices[i][index]* RotateMatrix;

			objects[objectindex].m_RotateMatrices[i][index] = RotateMatrix;

			//objects[objectindex].m_DefaultModelMatrices[i][index] = glm::translate(objects[objectindex].m_DefaultModelMatrices[i][index], objects[objectindex].m_Pos[index] / objects[objectindex].m_Scale);

			objects[objectindex].m_DefaultModelMatrices[i][index] = objects[objectindex].m_DefaultPosScaleMatrices[i][index] * objects[objectindex].m_RotateMatrices[i][index];
		}
		
		if (objects[objectindex].m_HaveAngle)
		{
			ChangeAngle(objectindex, index);
		}
		SetAABB(objectindex, index);
	}

	void Objects::ChangeRotateD(glm::vec3 ChangedRotate)
	{
		objects[m_Objectindex].m_Rotate[m_index] = ChangedRotate;
		objects[m_Objectindex].m_RotateQuaternion[m_index] = glm::qua<float>(ChangedRotate);

		for (int i = 0; i < objects[m_Objectindex].m_Model->meshes.size(); i++)
		{
			//glm::qua<float> Quaternion = glm::qua<float>(ChangedRotate);
			glm::mat4 RotateMatrix = glm::mat4(1.0f);
			RotateMatrix = glm::mat4_cast(objects[m_Objectindex].m_RotateQuaternion[m_index]) * RotateMatrix;
			//objects[objectindex].m_DefaultModelMatrices[i][index] = glm::translate(objects[objectindex].m_DefaultModelMatrices[i][index], -objects[objectindex].m_Pos[index] / objects[objectindex].m_Scale);

			//objects[objectindex].m_DefaultModelMatrices[i][index] = objects[objectindex].m_DefaultModelMatrices[i][index]* RotateMatrix;
			objects[m_Objectindex].m_RotateMatrices[i][m_index] = RotateMatrix;

			//objects[objectindex].m_DefaultModelMatrices[i][index] = glm::translate(objects[objectindex].m_DefaultModelMatrices[i][index], objects[objectindex].m_Pos[index] / objects[objectindex].m_Scale);

			objects[m_Objectindex].m_DefaultModelMatrices[i][m_index] = objects[m_Objectindex].m_DefaultPosScaleMatrices[i][m_index] * objects[m_Objectindex].m_RotateMatrices[i][m_index];
		}

		if (objects[m_Objectindex].m_HaveAngle)
		{
			ChangeAngle(m_Objectindex, m_index);
		}
		SetAABB(m_Objectindex, m_index);
	}

	void Objects::ChangeHandPos(glm::vec3 ChangedHandPos, int objectindex, int index)
	{
		objects[objectindex].m_HandPos[index] = ChangedHandPos;
	}

	void Objects::ChangeHandPos(glm::vec3 ChangedHandPos)
	{
		objects[m_Objectindex].m_HandPos[m_index] = ChangedHandPos;

	}

	void Objects::ChangeHandEular(glm::vec3 ChangedHandEular, int objectindex, int index)
	{
		objects[objectindex].m_HandEular[index] = ChangedHandEular;
	}

	void Objects::ChangeHandEular(glm::vec3 ChangedHandEular)
	{
		objects[m_Objectindex].m_HandEular[m_index] = ChangedHandEular;

	}

	void Objects::SetChoosedIndex(int ObjectIndex, int index)
	{
		m_index = index;
		m_Objectindex = ObjectIndex;
	}

	bool Objects::CheckCollision(int ObjectIndex, int index, glm::vec3 CheckPos)
	{
		if (CheckPos.x > objects[ObjectIndex].m_AABBMinPos[index].x&& CheckPos.x < objects[ObjectIndex].m_AABBMaxPos[index].x)
		{
			if (CheckPos.y > objects[ObjectIndex].m_AABBMinPos[index].y&& CheckPos.y < objects[ObjectIndex].m_AABBMaxPos[index].y)
			{
				if (CheckPos.z > objects[ObjectIndex].m_AABBMinPos[index].z&& CheckPos.z < objects[ObjectIndex].m_AABBMaxPos[index].z)
				{
					return true;
				}
			}
		}
		return false;
	}

	void Objects::SetAABB(int ObjectIndex,int index)
	{
		glm::vec4 vp = glm::vec4(0.0f);
		if (objects[ObjectIndex].m_HaveAngle)
		{
			vp = objects[ObjectIndex].m_ModelMatrices[0][index] * glm::vec4(objects[ObjectIndex].m_Model->meshes[0].vertices[0].Position, 1.0f);
		}
		else
		{
			vp = objects[ObjectIndex].m_DefaultModelMatrices[0][index] * glm::vec4(objects[ObjectIndex].m_Model->meshes[0].vertices[0].Position, 1.0f);

		}

		objects[ObjectIndex].m_AABBMinPos[index] = glm::vec3(vp.x, vp.y, vp.z);
		objects[ObjectIndex].m_AABBMaxPos[index] = glm::vec3(vp.x, vp.y, vp.z);


		for (int i = 0; i < objects[ObjectIndex].m_Model->meshes.size(); i++)
		{
			for (int j = 0; j < objects[ObjectIndex].m_Model->meshes[i].vertices.size(); j++)
			{
				glm::vec4 thisvp = glm::vec4(0.0f);
				if (objects[ObjectIndex].m_HaveAngle)
				{
					thisvp = objects[ObjectIndex].m_ModelMatrices[i][index] * glm::vec4(objects[ObjectIndex].m_Model->meshes[i].vertices[j].Position, 1.0f);
				}
				else
				{
					thisvp = objects[ObjectIndex].m_DefaultModelMatrices[i][index] * glm::vec4(objects[ObjectIndex].m_Model->meshes[i].vertices[j].Position, 1.0f);

				}
				if (thisvp.x < objects[ObjectIndex].m_AABBMinPos[index].x)
				{
					objects[ObjectIndex].m_AABBMinPos[index].x = thisvp.x;
				}
				if (thisvp.x > objects[ObjectIndex].m_AABBMaxPos[index].x)
				{
					objects[ObjectIndex].m_AABBMaxPos[index].x = thisvp.x;
				}
				if (thisvp.y < objects[ObjectIndex].m_AABBMinPos[index].y)
				{
					objects[ObjectIndex].m_AABBMinPos[index].y = thisvp.y;
				}
				if (thisvp.y > objects[ObjectIndex].m_AABBMaxPos[index].y)
				{
					objects[ObjectIndex].m_AABBMaxPos[index].y = thisvp.y;
				}
				if (thisvp.z < objects[ObjectIndex].m_AABBMinPos[index].z)
				{
					objects[ObjectIndex].m_AABBMinPos[index].z = thisvp.z;
				}
				if (thisvp.z > objects[ObjectIndex].m_AABBMaxPos[index].z)
				{
					objects[ObjectIndex].m_AABBMaxPos[index].z = thisvp.z;
				}
			}
		}
		
		
	}


	Hazel::ControlMode Objects::GetControlMode()
	{
		if (m_Objectindex > -1)
		{
			return objects[m_Objectindex].m_ControlMode;
		}
		else
		{
			return ControlMode::None;
		}
	}

	int* Objects::GetControlModeAddress()
	{
		if (m_Objectindex > -1)
		{
			return (int*)&objects[m_Objectindex].m_ControlMode;
		}
		else
		{
			return nullptr;
		}
	}

	bool Objects::SaveScene(int SaveAddress)
	{

		//Objects
		// 准备根据格式造字符串流
		std::stringstream fmt;
		// 造字符串流
		fmt << "res/save/Objects" << SaveAddress << ".json";
		std::string namej = fmt.str();

		std::fstream file(namej, std::ios::out);

		if (!file.is_open())
		{
			std::cout << "can not open json file to write." << std::endl;
			return false;
		}

		rapidjson::StringBuffer strBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(strBuffer);

		writer.StartObject();

		writer.Key("Objects");
		writer.StartArray();//层1
		int i = 0;
		for (Object object : objects)
		{
			writer.StartObject();//层2
			writer.Key("index");
			writer.Int(i);
			writer.Key("name");
			writer.String(object.m_Name.c_str());
			writer.Key("amount");
			writer.Int(object.m_Amount);
			writer.Key("filepath");
			writer.String(object.m_Model->m_path.c_str());
			writer.Key("haveangle");
			writer.Bool(object.m_HaveAngle);
			writer.Key("scale");
			writer.StartArray();
			writer.Double(object.m_Scale.x);
			writer.Double(object.m_Scale.y);
			writer.Double(object.m_Scale.z);
			writer.EndArray();
			writer.EndObject();
			i++;
		}
		writer.EndArray();
		writer.EndObject();
		file << strBuffer.GetString() << std::endl;



		//One Object

		// 准备根据格式造字符串流
		std::stringstream fmt1;
		// 造字符串流
		fmt1 << "res/save/ObjectsDetail" << SaveAddress << ".json";
		std::string namej1 = fmt1.str();

		std::fstream file1(namej1, std::ios::out);

		if (!file1.is_open())
		{
			std::cout << "can not open json file to write." << std::endl;
			return false;
		}

		rapidjson::StringBuffer strBuffer1;
		rapidjson::Writer<rapidjson::StringBuffer> writer1(strBuffer1);

		writer1.StartObject();

		writer1.Key("ObjectsDetail");
		writer1.StartArray();//层1
		for(Object object : objects)
		{
			
			writer1.StartObject();//层2
			writer1.Key(object.m_Name.c_str());
			writer1.StartArray();//层3
			for (int i = 0; i< object.m_Amount; i++)
			{
				writer1.StartObject();//层4
				writer1.Key("index");
				writer1.Int(i);
				writer1.Key("pos");
				writer1.StartArray();
				writer1.Double(object.m_Pos[i].x);
				writer1.Double(object.m_Pos[i].y);
				writer1.Double(object.m_Pos[i].z);
				writer1.EndArray();
				writer1.Key("rotate");
				writer1.StartArray();
				//writer1.Double(object.m_RotateQuaternion[i].w);
				//writer1.Double(object.m_RotateQuaternion[i].x);
				//writer1.Double(object.m_RotateQuaternion[i].y);
				//writer1.Double(object.m_RotateQuaternion[i].z);
				writer1.Double(object.m_Rotate[i].x);
				writer1.Double(object.m_Rotate[i].y);
				writer1.Double(object.m_Rotate[i].z);
				writer1.EndArray();
				
				writer1.Key("angle");
				writer1.StartArray();
				for (std::vector<float> m_angle : object.m_Angle)
				{
					writer1.Double(m_angle[i]);
				}
				writer1.EndArray();

				writer1.Key("poseular");
				writer1.StartArray();
				writer1.Double(object.m_HandPos[i].x);
				writer1.Double(object.m_HandPos[i].y);
				writer1.Double(object.m_HandPos[i].z);
				writer1.Double(object.m_HandEular[i].x);
				writer1.Double(object.m_HandEular[i].y);
				writer1.Double(object.m_HandEular[i].z);
				writer1.EndArray();
				
				if(object.m_Anima[i].HaveAnimation)
				{
					writer1.Key("animation");
					writer1.StartArray();
					for (int j = 0; j < object.m_Anima[i].GetPathKeySize(); j++)
					{
						writer1.StartObject();

						writer1.Key("a_pos");
						writer1.StartArray();
						writer1.Double(object.m_Anima[i].GetPathKeyPos(j).x);
						writer1.Double(object.m_Anima[i].GetPathKeyPos(j).y);
						writer1.Double(object.m_Anima[i].GetPathKeyPos(j).z);
						writer1.EndArray();

						writer1.Key("a_rotate");
						writer1.StartArray();
						writer1.Double(object.m_Anima[i].GetPathKeyRotate(j).x);
						writer1.Double(object.m_Anima[i].GetPathKeyRotate(j).y);
						writer1.Double(object.m_Anima[i].GetPathKeyRotate(j).z);
						writer1.EndArray();

						writer1.Key("a_handpos");
						writer1.StartArray();
						writer1.Double(object.m_Anima[i].GetPathKeyHandPos(j).x);
						writer1.Double(object.m_Anima[i].GetPathKeyHandPos(j).y);
						writer1.Double(object.m_Anima[i].GetPathKeyHandPos(j).z);
						writer1.EndArray();

						writer1.Key("a_handeular");
						writer1.StartArray();
						writer1.Double(object.m_Anima[i].GetPathKeyHandEular(j).x);
						writer1.Double(object.m_Anima[i].GetPathKeyHandEular(j).y);
						writer1.Double(object.m_Anima[i].GetPathKeyHandEular(j).z);
						writer1.EndArray();

						if (j > 0)
						{
							writer1.Key("a_pathtime");

							writer1.Double(object.m_Anima[i].GetPathKeyTime(j - 1));

							writer1.Key("a_pathmode");

							writer1.Int(object.m_Anima[i].GetPathMode(j - 1));


							writer1.Key("a_circlecenter");
							writer1.StartArray();
							writer1.Double(object.m_Anima[i].GetPathCircleCenter(j-1).x);
							writer1.Double(object.m_Anima[i].GetPathCircleCenter(j-1).y);
							writer1.Double(object.m_Anima[i].GetPathCircleCenter(j-1).z);
							writer1.EndArray();
						}

						writer1.EndObject();
					}
					writer1.EndArray();

				}
				writer1.EndObject();
			}
			writer1.EndArray();
			writer1.EndObject();
		}
		writer1.EndArray();
		writer1.EndObject();
		file1 << strBuffer1.GetString() << std::endl;

		return true;
	}

	bool Objects::LoadScene(int LoadAddress)
	{
		// 准备根据格式造字符串流
		std::stringstream fmt;
		// 造字符串流
		fmt << "res/save/Objects" << LoadAddress << ".json";
		std::string namej = fmt.str();

		std::ifstream file(namej, std::ios::in);
		if (!file.is_open())
		{
			std::cout << "can not open json file to read." << std::endl;
			return false;
		}
		std::string data;
		std::stringstream ss;
		while (getline(file, data))
		{
			ss << data;
		}


		std::string ObjectsData = ss.str();

		//创建解析对象
		rapidjson::Document doc;
		//首先进行解析，没有解析错误才能进行具体字段的解析
		if (!doc.Parse(ObjectsData.data()).HasParseError())
		{
			if (doc.HasMember("Objects") && doc["Objects"].IsArray())
			{
				const rapidjson::Value& array = doc["Objects"];
				size_t len = array.Size();
				for (size_t i = 0; i < len; i++)
				{
					const rapidjson::Value& object = array[i];
					//为防止类型不匹配，一般会添加类型校验
					if (object.IsObject())
					{
						if (object.HasMember("index") && object["index"].IsInt())
						{
							load_objectindex = object["index"].GetInt();
						}
						if (object.HasMember("name") && object["name"].IsString())
						{
							load_name = object["name"].GetString();
						}
						if (object.HasMember("amount") && object["amount"].IsInt())
						{
							load_amount = object["amount"].GetInt();
						}
						if (object.HasMember("filepath") && object["filepath"].IsString())
						{
							load_filepath = object["filepath"].GetString();
						}
						if (object.HasMember("haveangle") && object["haveangle"].IsBool())
						{
							load_haveangle = object["haveangle"].GetBool();
						}
						if (object.HasMember("scale") && object["scale"].IsArray())
						{
							const rapidjson::Value& array1 = object["scale"];
							load_scale[0] = array1[0].GetDouble();
							load_scale[1] = array1[1].GetDouble();
							load_scale[2] = array1[2].GetDouble();
						}
						AddObject(load_name, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), load_scale, load_haveangle);
					}
				}
			}
			//1. 解析整数
// 			if (doc.HasMember("Int") && doc["Int"].IsInt())
// 			{
// 				cout << "Int = " << doc["Int"].GetInt() << endl;
// 			}
// 			//2. 解析浮点型
// 			if (doc.HasMember("Double") && doc["Double"].IsDouble())
// 			{
// 				cout << "Double = " << doc["Double"].GetDouble() << endl;
// 			}
// 			//3. 解析字符串
// 			if (doc.HasMember("String") && doc["String"].IsString())
// 			{
// 				cout << "String = " << doc["String"].GetString() << endl;
// 			}
// 			//4. 解析结构体
// 			if (doc.HasMember("Object") && doc["Object"].IsObject())
// 			{
// 				const rapidjson::Value& object = doc["Object"];
// 				if (object.HasMember("name") && object["name"].IsString())
// 				{
// 					cout << "Object.name = " << object["name"].GetString() << endl;
// 				}
// 				if (object.HasMember("age") && object["age"].IsInt())
// 				{
// 					cout << "Object.age = " << object["age"].GetInt() << endl;
// 				}
// 			}
// 			//5. 解析数组类型
// 			//5.1 整型数组类型
// 			if (doc.HasMember("IntArray") && doc["IntArray"].IsArray())
// 			{
// 				//5.1.1 将字段转换成为rapidjson::Value类型
// 				const rapidjson::Value& array = doc["IntArray"];
// 				//5.1.2 获取数组长度
// 				size_t len = array.Size();
// 				//5.1.3 根据下标遍历，注意将元素转换为相应类型，即需要调用GetInt()
// 				for (size_t i = 0; i < len; i++)
// 				{
// 					cout << "IntArray[" << i << "] = " << array[i].GetInt() << endl;
// 				}
// 			}
// 			//5.2 浮点型数组类型
// 			if (doc.HasMember("DoubleArray") && doc["DoubleArray"].IsArray())
// 			{
// 				const rapidjson::Value& array = doc["DoubleArray"];
// 				size_t len = array.Size();
// 				for (size_t i = 0; i < len; i++)
// 				{
// 					//为防止类型不匹配，一般会添加类型校验
// 					if (array[i].IsDouble())
// 					{
// 						cout << "DoubleArray[" << i << "] = " << array[i].GetDouble() << endl;
// 					}
// 				}
// 			}
// 			//5.3 字符串数组类型
// 			if (doc.HasMember("StringArray") && doc["StringArray"].IsArray())
// 			{
// 				const rapidjson::Value& array = doc["StringArray"];
// 				size_t len = array.Size();
// 				for (size_t i = 0; i < len; i++)
// 				{
// 					//为防止类型不匹配，一般会添加类型校验
// 					if (array[i].IsString())
// 					{
// 						cout << "StringArray[" << i << "] = " << array[i].GetString() << endl;
// 					}
// 				}
// 			}
// 			//5.4 混合型
// 			if (doc.HasMember("MixedArray") && doc["MixedArray"].IsArray())
// 			{
// 				const rapidjson::Value& array = doc["MixedArray"];
// 				size_t len = array.Size();
// 				for (size_t i = 0; i < len; i++)
// 				{
// 					//为防止类型不匹配，一般会添加类型校验
// 					if (array[i].IsString())
// 					{
// 						cout << "MixedArray[" << i << "] = " << array[i].GetString() << endl;
// 					}
// 					else if (array[i].IsBool())
// 					{
// 						cout << "MixedArray[" << i << "] = " << array[i].GetBool() << endl;
// 					}
// 					else if (array[i].IsInt())
// 					{
// 						cout << "MixedArray[" << i << "] = " << array[i].GetInt() << endl;
// 					}
// 					else if (array[i].IsDouble())
// 					{
// 						cout << "MixedArray[" << i << "] = " << array[i].GetDouble() << endl;
// 					}
// 				}
// 			}
// 			//5.5 结构体数组类型
// 			if (doc.HasMember("People") && doc["People"].IsArray())
// 			{
// 				const rapidjson::Value& array = doc["People"];
// 				size_t len = array.Size();
// 				for (size_t i = 0; i < len; i++)
// 				{
// 					const rapidjson::Value& object = array[i];
// 					//为防止类型不匹配，一般会添加类型校验
// 					if (object.IsObject())
// 					{
// 						cout << "ObjectArray[" << i << "]: ";
// 						if (object.HasMember("name") && object["name"].IsString())
// 						{
// 							cout << "name=" << object["name"].GetString();
// 						}
// 						if (object.HasMember("age") && object["age"].IsInt())
// 						{
// 							cout << ", age=" << object["age"].GetInt();
// 						}
// 						if (object.HasMember("sex") && object["sex"].IsBool())
// 						{
// 							cout << ", sex=" << (object["sex"].GetBool() ? "男" : "女") << endl;
// 						}
// 					}
// 				}
// 			}
// 		}
		/**
		 *    最后注意：因为rapidjson不会做安全校验，所以要自己做安全校验，以int整型为例
		 * “if(object.HasMember("age") && object["age"].IsInt()) {}”
		 * 这句校验很重要，既要校验有该子段，也要校验类型正确，否则会引发程序崩溃
		 */
		
		 // 准备根据格式造字符串流
		std::stringstream fmt1;
		// 造字符串流
		fmt1 << "res/save/ObjectsDetail" << LoadAddress << ".json";
		std::string namej1 = fmt1.str();

		std::ifstream file1(namej1, std::ios::in);
		if (!file1.is_open())
		{
			std::cout << "can not open json file to read." << std::endl;
			return false;
		}
		std::string data1;
		std::stringstream ss1;
		while (getline(file1, data1))
		{
			ss1 << data1;
		}


		std::string ObjectsDetailData = ss1.str();

		//创建解析对象
		rapidjson::Document doc1;
		//首先进行解析，没有解析错误才能进行具体字段的解析
		if (!doc1.Parse(ObjectsDetailData.data()).HasParseError())
		{
			if (doc1.HasMember("ObjectsDetail") && doc1["ObjectsDetail"].IsArray())
			{
				const rapidjson::Value& array2 = doc1["ObjectsDetail"];
				size_t len1 = array2.Size();
				for (size_t i = 0; i < len1; i++)//每种物体
				{
					const rapidjson::Value& object1 = array2[i];

					//为防止类型不匹配，一般会添加类型校验
					if (object1.IsObject())
					{
						if (object1.HasMember(objects[i].m_Name.c_str()) && object1[objects[i].m_Name.c_str()].IsArray())
						{
							const rapidjson::Value& array3 = object1[objects[i].m_Name.c_str()];
							size_t len2 = array3.Size();
							for (size_t j = 0; j < len2; j++)//每个重复物体
							{
								const rapidjson::Value& object2 = array3[j];
								//为防止类型不匹配，一般会添加类型校验
								if (object2.IsObject())
								{
									if (object2.HasMember("pos") && object2["pos"].IsArray())
									{
										const rapidjson::Value& array4 = object2["pos"];
										load_pos[0] = array4[0].GetDouble();
										load_pos[1] = array4[1].GetDouble();
										load_pos[2] = array4[2].GetDouble();
									}
									if (object2.HasMember("rotate") && object2["rotate"].IsArray())
									{
										const rapidjson::Value& array5 = object2["rotate"];
										load_rotate[0] = array5[0].GetDouble();
										load_rotate[1] = array5[1].GetDouble();
										load_rotate[2] = array5[2].GetDouble();
									}
									
									
									m_Objectindex = i;
									m_index = j;
									if (j > 0)
									{
										Load_AddAmount();
									}
									ChangePos(load_pos);
									ChangeRotate(load_rotate);
									if (object2.HasMember("angle") && object2["angle"].IsArray())
									{
										const rapidjson::Value& array6 = object2["angle"];

										size_t len3 = array6.Size();
										for (size_t k = 0; k < len3; k++)//每个关节
										{
											objects[m_Objectindex].m_Angle[k][m_index] = array6[k].GetDouble();
										}
									}
									if (objects[m_Objectindex].m_HaveAngle)
									{
										ChangeAngle();
									}
									if (object2.HasMember("poseular") && object2["poseular"].IsArray())
									{
										const rapidjson::Value& array7 = object2["poseular"];
										objects[i].m_HandPos[j].x = array7[0].GetDouble();
										objects[i].m_HandPos[j].y = array7[1].GetDouble();
										objects[i].m_HandPos[j].z = array7[2].GetDouble();
										objects[i].m_HandEular[j].x = array7[3].GetDouble();
										objects[i].m_HandEular[j].y = array7[4].GetDouble();
										objects[i].m_HandEular[j].z = array7[5].GetDouble();

									}

									if (object2.HasMember("animation") && object2["animation"].IsArray())
									{
										const rapidjson::Value& array8 = object2["animation"];
										size_t len3 = array8.Size();
										for (size_t k = 0; k < len3; k++)//每种物体
										{
											const rapidjson::Value& object3 = array8[k];

											//为防止类型不匹配，一般会添加类型校验
											if (object3.IsObject())
											{
												if (object3.HasMember("a_pos") && object3["a_pos"].IsArray())
												{
													const rapidjson::Value& array9 = object3["a_pos"];
													objects[i].m_Anima[j].SetPathPos(glm::vec3(array9[0].GetDouble(), array9[1].GetDouble(), array9[2].GetDouble()));
													
												}

												if (object3.HasMember("a_rotate") && object3["a_rotate"].IsArray())
												{
													const rapidjson::Value& array10 = object3["a_rotate"];
													objects[i].m_Anima[j].SetPathRotate(glm::vec3(array10[0].GetDouble(), array10[1].GetDouble(), array10[2].GetDouble()));

												}

												if (object3.HasMember("a_handpos") && object3["a_handpos"].IsArray())
												{
													const rapidjson::Value& array11 = object3["a_handpos"];
													objects[i].m_Anima[j].SetPathHandPos(glm::vec3(array11[0].GetDouble(), array11[1].GetDouble(), array11[2].GetDouble()));

												}

												if (object3.HasMember("a_handeular") && object3["a_handeular"].IsArray())
												{
													const rapidjson::Value& array12 = object3["a_handeular"];
													objects[i].m_Anima[j].SetPathHandEular(glm::vec3(array12[0].GetDouble(), array12[1].GetDouble(), array12[2].GetDouble()));

												}

												if (object3.HasMember("a_pathtime") && object3["a_pathtime"].IsDouble())
												{
													objects[i].m_Anima[j].SetPathTime(object3["a_pathtime"].GetDouble());
												}

												if (object3.HasMember("a_pathmode") && object3["a_pathmode"].IsInt())
												{
													PathMode temp = static_cast<PathMode>(object3["a_pathmode"].GetInt());
													objects[i].m_Anima[j].pathmode = static_cast<PathMode>(object3["a_pathmode"].GetInt());
												}

												if (object3.HasMember("a_circlecenter") && object3["a_circlecenter"].IsArray())
												{
													const rapidjson::Value& array13 = object3["a_circlecenter"];
													objects[i].m_Anima[j].SetPathMode(glm::vec3(array13[0].GetDouble(), array13[1].GetDouble(), array13[2].GetDouble()));

												}
											}
												

											
										}
									}
										
									
								}
							}

						}
					}
				}
			}

		}
		}
		return true;
	}
}


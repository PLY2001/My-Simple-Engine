#include "hzpch.h"
#include "Belt.h"
#include "ABBIRB120.h"

namespace Hazel {
	Belt::Belt(glm::vec3 Pos, glm::vec3 Rotation, glm::vec3 Scale, std::shared_ptr<Model>& model) : Scale(Scale), model(model)
	{
		m_Pos.push_back(Pos);
		m_Rotate.push_back(Rotation);

	}

	void Belt::InitModelMatrices()
	{
		//创建变换矩阵
		//ModelMatrices.resize(model->meshes.size());//[i][j]，i是关节索引，j是个体索引
		DefaultModelMatrices.resize(model->meshes.size());
		for (int i = 0; i < model->meshes.size(); i++)
		{
			//ModelMatrices[i].push_back(model->mModelMatrix);
			//ModelMatrices[i].back() = glm::scale(ModelMatrices[i].back(), Scale);
			DefaultModelMatrices[i].push_back(model->mModelMatrix);
			DefaultModelMatrices[i].back() = glm::scale(DefaultModelMatrices[i].back(), Scale);
		}

		AABBMinPos.push_back(m_Pos[0]);
		AABBMaxPos.push_back(m_Pos[0]);
		SetAABB(0);
	}



	void Belt::AddAmount()
	{
		//增加模型
		Amount++;
		if (int increase = Amount - DefaultModelMatrices[0].size() > 0)
		{
			for (int j = 0; j < increase; j++)
			{
				m_Pos.push_back(glm::vec3((Amount - 1) * 10.0f, 0.0f, 0.0f));
				m_Rotate.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
			}
			for (int i = 0; i < model->meshes.size(); i++)
			{
				for (int j = 0; j < increase; j++)
				{
					//ModelMatrices[i].push_back(ModelMatrix(glm::vec3(m_Pos.back())).matrix);
					//ModelMatrices[i].back() = glm::scale(ModelMatrices[i].back(), Scale);

					DefaultModelMatrices[i].push_back(ModelMatrix(glm::vec3(m_Pos.back())).matrix);
					DefaultModelMatrices[i].back() = glm::scale(DefaultModelMatrices[i].back(), Scale);
				}
			}

		}
		AABBMinPos.push_back(m_Pos[Amount - 1]);
		AABBMaxPos.push_back(m_Pos[Amount - 1]);
		
		SetAABB(Amount - 1);
	}

	int Belt::GetAmount()
	{
		return Amount;
	}

	

	glm::vec3 Belt::GetAABBMaxPos()
	{
		return AABBMaxPos[m_index];
	}

	glm::vec3 Belt::GetAABBMinPos()
	{
		return AABBMinPos[m_index];
	}



	glm::vec3 Belt::GetScale()
	{
		return Scale;
	}

	glm::vec3 Belt::GetPos()
	{
		return m_Pos[m_index];
	}


	glm::vec3 Belt::GetRotate()
	{
		return m_Rotate[m_index];
	}

	void Belt::ChangePos(glm::vec3 ChangedPos)
	{
		m_Pos[m_index] += ChangedPos;
		for (int i = 0; i < model->meshes.size(); i++)
		{
			//ModelMatrices[i][index] = glm::translate(ModelMatrices[i][index],ChangedPos/Scale);
			DefaultModelMatrices[i][m_index] = glm::rotate(DefaultModelMatrices[i][m_index], -m_Rotate[m_index].x, glm::vec3(1.0f, 0.0f, 0.0f));
			DefaultModelMatrices[i][m_index] = glm::rotate(DefaultModelMatrices[i][m_index], -m_Rotate[m_index].y, glm::vec3(0.0f, 1.0f, 0.0f));
			DefaultModelMatrices[i][m_index] = glm::rotate(DefaultModelMatrices[i][m_index], -m_Rotate[m_index].z, glm::vec3(0.0f, 0.0f, 1.0f));
			DefaultModelMatrices[i][m_index] = glm::translate(DefaultModelMatrices[i][m_index], ChangedPos / Scale);
			DefaultModelMatrices[i][m_index] = glm::rotate(DefaultModelMatrices[i][m_index], m_Rotate[m_index].z, glm::vec3(0.0f, 0.0f, 1.0f));
			DefaultModelMatrices[i][m_index] = glm::rotate(DefaultModelMatrices[i][m_index], m_Rotate[m_index].y, glm::vec3(0.0f, 1.0f, 0.0f));
			DefaultModelMatrices[i][m_index] = glm::rotate(DefaultModelMatrices[i][m_index], m_Rotate[m_index].x, glm::vec3(1.0f, 0.0f, 0.0f));
		}

		SetAABB(m_index);
	}

	void Belt::ChangeRotate(glm::vec3 ChangedRotate, int RotateAxis)
	{

		m_Rotate[m_index] += ChangedRotate;
		for (int i = 0; i < model->meshes.size(); i++)
		{
			//ModelMatrices[i][index] = glm::translate(ModelMatrices[i][index],ChangedPos/Scale);
			glm::vec3 RotateAxisVec3 = glm::vec3(0.0f);
			RotateAxisVec3[RotateAxis] = 1.0f;
			//DefaultModelMatrices[i][m_index] = glm::translate(DefaultModelMatrices[i][m_index], -m_Pos[m_index] / Scale);
			DefaultModelMatrices[i][m_index] = glm::rotate(DefaultModelMatrices[i][m_index], ChangedRotate[RotateAxis], RotateAxisVec3);
			//DefaultModelMatrices[i][m_index] = glm::translate(DefaultModelMatrices[i][m_index], m_Pos[m_index] / Scale);
		}

		SetAABB(m_index);
	}

	void Belt::SetChoosedIndex(int index)
	{
		m_index = index;
	}

	bool Belt::CheckCollision(int index, glm::vec3 CheckPos)
	{
		if (CheckPos.x > AABBMinPos[index].x&& CheckPos.x < AABBMaxPos[index].x)
		{
			if (CheckPos.y > AABBMinPos[index].y&& CheckPos.y < AABBMaxPos[index].y)
			{
				if (CheckPos.z > AABBMinPos[index].z&& CheckPos.z < AABBMaxPos[index].z)
				{
					return true;
				}
			}
		}
		return false;
	}

	void Belt::SetAABB(int index)
	{
		// 	for(int n = 0; n < Amount; n++)
		// 	{
		glm::vec4 vp = DefaultModelMatrices[0][index] * glm::vec4(model->meshes[0].vertices[0].Position, 1.0f);

		AABBMinPos[index] = glm::vec3(vp.x, vp.y, vp.z);
		AABBMaxPos[index] = glm::vec3(vp.x, vp.y, vp.z);


		for (int i = 0; i < model->meshes.size(); i++)
		{
			for (int j = 0; j < model->meshes[i].vertices.size(); j++)
			{
				glm::vec4 thisvp = DefaultModelMatrices[i][index] * glm::vec4(model->meshes[i].vertices[j].Position, 1.0f);
				if (thisvp.x < AABBMinPos[index].x)
				{
					AABBMinPos[index].x = thisvp.x;
				}
				if (thisvp.x > AABBMaxPos[index].x)
				{
					AABBMaxPos[index].x = thisvp.x;
				}
				if (thisvp.y < AABBMinPos[index].y)
				{
					AABBMinPos[index].y = thisvp.y;
				}
				if (thisvp.y > AABBMaxPos[index].y)
				{
					AABBMaxPos[index].y = thisvp.y;
				}
				if (thisvp.z < AABBMinPos[index].z)
				{
					AABBMinPos[index].z = thisvp.z;
				}
				if (thisvp.z > AABBMaxPos[index].z)
				{
					AABBMaxPos[index].z = thisvp.z;
				}
			}
		}
		//}
	}


}


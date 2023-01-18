#include "hzpch.h"
#include "Objects.h"

namespace Hazel {

	Objects::Objects(std::map<std::string, std::shared_ptr<Model>>& modelmap)
	{
		m_modelmap = modelmap;
	}

	void Objects::AddObject(std::string name, glm::vec3 Pos, glm::vec3 Rotation, glm::vec3 Scale, bool hasAngle)
	{
		ObjectAmount++;
		m_Pos.resize(ObjectAmount);
		m_Pos[ObjectAmount-1].push_back(Pos);
		m_Rotate.resize(ObjectAmount);
		m_Rotate.back().push_back(Rotation);
		m_Scale.push_back(Scale);
		m_model.push_back(m_modelmap[name]);
		haveAngle.push_back(hasAngle);

		ObjectsMap.push_back(name);
		
		Amount.push_back(1);
		if (hasAngle)
		{
			ModelMatrices.resize(ObjectAmount);
			controlmode.push_back(ControlMode::Angles);
		}
		else
		{
			controlmode.push_back(ControlMode::None);

		}
		DefaultModelMatrices.resize(ObjectAmount);
		AABBMinPos.resize(ObjectAmount);
		AABBMaxPos.resize(ObjectAmount);

		Angle.resize(ObjectAmount);
		Angle.back().resize(m_modelmap[name]->meshes.size()-1);//[i][j][k]，i是物体种类，j是关节索引，k是重复物体索引
		for (int i = 0; i < m_modelmap[name]->meshes.size()-1; i++)
		{
			Angle[ObjectAmount-1][i].push_back(0);
		}
		Pos_Eular.resize(ObjectAmount);
		Pos_Eular.back().push_back({-5.18f, 6.3f, 0.0f, 0.0f, 0.0f, 0.0f});
		InitModelMatrices(ObjectAmount - 1);
		
	}

	void Objects::InitModelMatrices(int ObjectIndex)
	{
		//创建变换矩阵
		
		if (haveAngle[ObjectIndex])
		{
			ModelMatrices[ObjectIndex].resize(m_model[ObjectIndex]->meshes.size());//[i][j][k]，i是种类索引，j关节索引，k是重复个体索引
		}
		DefaultModelMatrices[ObjectIndex].resize(m_model[ObjectIndex]->meshes.size());
		for (int i = 0; i < m_model[ObjectIndex]->meshes.size(); i++)
		{
			if (haveAngle[ObjectIndex])
			{
				ModelMatrices[ObjectIndex][i].push_back(m_model[ObjectIndex]->mModelMatrix);
				ModelMatrices[ObjectIndex][i].back() = glm::translate(ModelMatrices[ObjectIndex][i].back(), m_Pos[ObjectIndex][0]);
				ModelMatrices[ObjectIndex][i].back() = glm::scale(ModelMatrices[ObjectIndex][i].back(), m_Scale[ObjectIndex]);
			}
			DefaultModelMatrices[ObjectIndex][i].push_back(m_model[ObjectIndex]->mModelMatrix);
			DefaultModelMatrices[ObjectIndex][i].back() = glm::translate(DefaultModelMatrices[ObjectIndex][i].back(), m_Pos[ObjectIndex][0]);
			DefaultModelMatrices[ObjectIndex][i].back() = glm::scale(DefaultModelMatrices[ObjectIndex][i].back(), m_Scale[ObjectIndex]);
		}


		AABBMinPos[ObjectIndex].push_back(m_Pos[ObjectIndex][0]);
		AABBMaxPos[ObjectIndex].push_back(m_Pos[ObjectIndex][0]);
		SetAABB(ObjectIndex,0);
		
	}



	glm::mat4* Objects::GetModelMatrix(int ObjectIndex, int index)
	{
		if (haveAngle[ObjectIndex])
		{
			return ModelMatrices[ObjectIndex][index].data();
		}
		else
		{
			return DefaultModelMatrices[ObjectIndex][index].data();
		}
	}

	void Objects::AddAmount()
	{
		
		//增加模型
		Amount[m_Objectindex]++;
		if (m_index < 0)
		{
			m_index = Amount[m_Objectindex] - 1;
		}
		if (int increase = Amount[m_Objectindex] - DefaultModelMatrices[m_Objectindex][0].size() > 0)
		{
			for (int j = 0; j < increase; j++)
			{
				m_Pos[m_Objectindex].push_back(m_Pos[m_Objectindex][m_index]);
				m_Rotate[m_Objectindex].push_back(m_Rotate[m_Objectindex][m_index]);
			}
			for (int i = 0; i < m_model[m_Objectindex]->meshes.size(); i++)
			{
				for (int j = 0; j < increase; j++)
				{
					if (haveAngle[m_Objectindex])
					{
						ModelMatrices[m_Objectindex][i].push_back(ModelMatrix(glm::vec3(m_Pos[m_Objectindex].back())).matrix);
						ModelMatrices[m_Objectindex][i].back() = glm::rotate(ModelMatrices[m_Objectindex][i].back(), m_Rotate[m_Objectindex].back().y, glm::vec3(0.0f,1.0f,0.0f));
						ModelMatrices[m_Objectindex][i].back() = glm::scale(ModelMatrices[m_Objectindex][i].back(), m_Scale[m_Objectindex]);
					}
					DefaultModelMatrices[m_Objectindex][i].push_back(ModelMatrix(glm::vec3(m_Pos[m_Objectindex].back())).matrix);
					DefaultModelMatrices[m_Objectindex][i].back() = glm::rotate(DefaultModelMatrices[m_Objectindex][i].back(), m_Rotate[m_Objectindex].back().y, glm::vec3(0.0f, 1.0f, 0.0f));
					DefaultModelMatrices[m_Objectindex][i].back() = glm::scale(DefaultModelMatrices[m_Objectindex][i].back(), m_Scale[m_Objectindex]);
				}
			}

		}
		AABBMinPos[m_Objectindex].push_back(m_Pos[m_Objectindex][Amount[m_Objectindex] - 1]);
		AABBMaxPos[m_Objectindex].push_back(m_Pos[m_Objectindex][Amount[m_Objectindex] - 1]);
		
		for (int i = 0; i < m_model[m_Objectindex]->meshes.size()-1; i++)
		{
			Angle[m_Objectindex][i].push_back(0);
		}
		Pos_Eular[m_Objectindex].push_back({ -5.18f, 6.3f, 0.0f, 0.0f, 0.0f, 0.0f });

		
		SetAABB(m_Objectindex,Amount[m_Objectindex] - 1);
		m_index = Amount[m_Objectindex] - 1;
	}

	void Objects::AddAmount(std::string name)
	{
		auto it = std::find(ObjectsMap.begin(), ObjectsMap.end(), name);
		int i = 0;
		for (auto itt = ObjectsMap.begin(); itt != ObjectsMap.end(); itt++)
		{
			if (itt == it)
			{
				m_Objectindex = i;
				break;
			}
			i++;
		}
		
		AddAmount();
	}

	void Objects::ReduceAmount()
	{
		Amount[m_Objectindex]--;
		if (m_index < 0)
		{
			m_index = Amount[m_Objectindex];
		}
		if (int decrease = DefaultModelMatrices[m_Objectindex][0].size() - Amount[m_Objectindex] > 0)
		{
			for (int j = 0; j < decrease; j++)
			{
				m_Pos[m_Objectindex].erase(m_Pos[m_Objectindex].begin() + m_index);
				m_Rotate[m_Objectindex].erase(m_Rotate[m_Objectindex].begin() + m_index);
			}
			for (int i = 0; i < m_model[m_Objectindex]->meshes.size(); i++)
			{
				for (int j = 0; j < decrease; j++)
				{
					if (haveAngle[m_Objectindex])
					{
						ModelMatrices[m_Objectindex][i].erase(ModelMatrices[m_Objectindex][i].begin() + m_index);
					}
					DefaultModelMatrices[m_Objectindex][i].erase(DefaultModelMatrices[m_Objectindex][i].begin() + m_index);
				}
			}

		}
		AABBMinPos[m_Objectindex].erase(AABBMinPos[m_Objectindex].begin() + m_index);
		AABBMaxPos[m_Objectindex].erase(AABBMaxPos[m_Objectindex].begin() + m_index);

		for (int i = 0; i < m_model[m_Objectindex]->meshes.size() - 1; i++)
		{
			Angle[m_Objectindex][i].erase(Angle[m_Objectindex][i].begin() + m_index);
		}
		Pos_Eular[m_Objectindex].erase(Pos_Eular[m_Objectindex].begin() + m_index);

		m_index = -1;
	}

	void Objects::Load_AddAmount()
	{
		
		//增加模型
		Amount[m_Objectindex]++;
		//m_index = Amount[m_Objectindex] - 1;
		if (int increase = Amount[m_Objectindex] - DefaultModelMatrices[m_Objectindex][0].size() > 0)
		{
			for (int j = 0; j < increase; j++)
			{
				m_Pos[m_Objectindex].push_back(glm::vec3(0.0f));
				m_Rotate[m_Objectindex].push_back(glm::vec3(0.0f));
			}
			for (int i = 0; i < m_model[m_Objectindex]->meshes.size(); i++)
			{
				for (int j = 0; j < increase; j++)
				{
					if (haveAngle[m_Objectindex])
					{
						ModelMatrices[m_Objectindex][i].push_back(ModelMatrix(glm::vec3(m_Pos[m_Objectindex].back())).matrix);
						ModelMatrices[m_Objectindex][i].back() = glm::rotate(ModelMatrices[m_Objectindex][i].back(), m_Rotate[m_Objectindex].back().y, glm::vec3(0.0f, 1.0f, 0.0f));
						ModelMatrices[m_Objectindex][i].back() = glm::scale(ModelMatrices[m_Objectindex][i].back(), m_Scale[m_Objectindex]);
					}
					DefaultModelMatrices[m_Objectindex][i].push_back(ModelMatrix(glm::vec3(m_Pos[m_Objectindex].back())).matrix);
					DefaultModelMatrices[m_Objectindex][i].back() = glm::rotate(DefaultModelMatrices[m_Objectindex][i].back(), m_Rotate[m_Objectindex].back().y, glm::vec3(0.0f, 1.0f, 0.0f));
					DefaultModelMatrices[m_Objectindex][i].back() = glm::scale(DefaultModelMatrices[m_Objectindex][i].back(), m_Scale[m_Objectindex]);
				}
			}

		}
		AABBMinPos[m_Objectindex].push_back(m_Pos[m_Objectindex][Amount[m_Objectindex] - 1]);
		AABBMaxPos[m_Objectindex].push_back(m_Pos[m_Objectindex][Amount[m_Objectindex] - 1]);

		for (int i = 0; i < m_model[m_Objectindex]->meshes.size() - 1; i++)
		{
			Angle[m_Objectindex][i].push_back(0);
		}
		Pos_Eular[m_Objectindex].push_back({ -5.18f, 6.3f, 0.0f, 0.0f, 0.0f, 0.0f });


		SetAABB(m_Objectindex, Amount[m_Objectindex] - 1);
	}

	int Objects::GetAmount(int ObjectIndex)
	{
		return Amount[ObjectIndex];
	}

	int Objects::GetMyAmount()
	{
		return Amount[m_Objectindex];
	}

	int Objects::GetObjectAmount()
	{
		return ObjectAmount;
	}

	void Objects::ChangeAngle()
	{
		//归位
		for (int j = 0; j < m_model[m_Objectindex]->meshes.size(); j++)
		{
			// 		for (int i = 0; i < Amount; i++)
			// 		{
			ModelMatrices[m_Objectindex][j][m_index] = DefaultModelMatrices[m_Objectindex][j][m_index];
			// 		}

		}


		for (int j = 1; j < m_model[m_Objectindex]->meshes.size(); j++)
		{
			// 		for (int i = 0; i < Amount; i++)
			// 		{
			if (j > 0)
			{
				ModelMatrices[m_Objectindex][j][m_index] = glm::rotate(ModelMatrices[m_Objectindex][j][m_index], glm::radians(Angle[m_Objectindex][0][m_index]), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (j > 1)
			{

				ModelMatrices[m_Objectindex][j][m_index] = glm::translate(ModelMatrices[m_Objectindex][j][m_index], glm::vec3(0.0f, 290.0f, 0.0f));
				ModelMatrices[m_Objectindex][j][m_index] = glm::rotate(ModelMatrices[m_Objectindex][j][m_index], glm::radians(Angle[m_Objectindex][1][m_index]), glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrices[m_Objectindex][j][m_index] = glm::translate(ModelMatrices[m_Objectindex][j][m_index], glm::vec3(0.0f, -290.0f, 0.0f));

			}
			if (j > 2)
			{
				ModelMatrices[m_Objectindex][j][m_index] = glm::translate(ModelMatrices[m_Objectindex][j][m_index], glm::vec3(0.0f, 560.0f, 0.0f));
				ModelMatrices[m_Objectindex][j][m_index] = glm::rotate(ModelMatrices[m_Objectindex][j][m_index], glm::radians(Angle[m_Objectindex][2][m_index]), glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrices[m_Objectindex][j][m_index] = glm::translate(ModelMatrices[m_Objectindex][j][m_index], glm::vec3(0.0f, -560.0f, 0.0f));
			}
			if (j > 3)
			{
				ModelMatrices[m_Objectindex][j][m_index] = glm::translate(ModelMatrices[m_Objectindex][j][m_index], glm::vec3(0.0f, 630.0f, 0.0f));
				ModelMatrices[m_Objectindex][j][m_index] = glm::rotate(ModelMatrices[m_Objectindex][j][m_index], glm::radians(Angle[m_Objectindex][3][m_index]), glm::vec3(1.0f, 0.0f, 0.0f));
				ModelMatrices[m_Objectindex][j][m_index] = glm::translate(ModelMatrices[m_Objectindex][j][m_index], glm::vec3(0.0f, -630.0f, 0.0f));
			}
			if (j > 4)
			{
				ModelMatrices[m_Objectindex][j][m_index] = glm::translate(ModelMatrices[m_Objectindex][j][m_index], glm::vec3(-302.0f, 630.0f, 0.0f));
				ModelMatrices[m_Objectindex][j][m_index] = glm::rotate(ModelMatrices[m_Objectindex][j][m_index], glm::radians(Angle[m_Objectindex][4][m_index]), glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrices[m_Objectindex][j][m_index] = glm::translate(ModelMatrices[m_Objectindex][j][m_index], glm::vec3(302.0f, -630.0f, 0.0f));
			}
			if (j > 5)
			{
				ModelMatrices[m_Objectindex][j][m_index] = glm::translate(ModelMatrices[m_Objectindex][j][m_index], glm::vec3(-302.0f, 630.0f, 0.0f));
				ModelMatrices[m_Objectindex][j][m_index] = glm::rotate(ModelMatrices[m_Objectindex][j][m_index], glm::radians(Angle[m_Objectindex][5][m_index]), glm::vec3(1.0f, 0.0f, 0.0f));
				ModelMatrices[m_Objectindex][j][m_index] = glm::translate(ModelMatrices[m_Objectindex][j][m_index], glm::vec3(302.0f, -630.0f, 0.0f));
			}

			//}

		}
		SetAABB(m_Objectindex,m_index);

	}

	float* Objects::SetAngle(int Axis)
	{
		return &Angle[m_Objectindex][Axis - 1][m_index];
	}

	float* Objects::SetPos_Eular(int index)
	{
		return &Pos_Eular[m_Objectindex][m_index][index];
	}

	bool Objects::SolveAngle()
	{
		glm::vec3 Pos = glm::vec3(Pos_Eular[m_Objectindex][m_index][0], Pos_Eular[m_Objectindex][m_index][1], Pos_Eular[m_Objectindex][m_index][2])/m_Scale[m_Objectindex];
		glm::vec3 Eular = glm::vec3(Pos_Eular[m_Objectindex][m_index][3], Pos_Eular[m_Objectindex][m_index][4], Pos_Eular[m_Objectindex][m_index][5]);
		glm::mat4 backwardTranslate = glm::mat4(1);;
		backwardTranslate = glm::translate(backwardTranslate, glm::vec3(216.0f, 0.0f, 0.0f));//夹具中心到上一轴的距离为72+144


		glm::mat4 backwardEular = glm::mat4(1);
		backwardEular = glm::translate(backwardEular, Pos);
		backwardEular = glm::rotate(backwardEular, glm::radians(Eular.x), glm::vec3(1, 0, 0));
		backwardEular = glm::rotate(backwardEular, glm::radians(Eular.y), glm::vec3(0, 1, 0));
		backwardEular = glm::rotate(backwardEular, glm::radians(Eular.z), glm::vec3(0, 0, 1));
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
		// 	ImGui::Text("Angle2 = %.3f", SolvedAngles[1]);
		// 	ImGui::Text("Angle3 = %.3f", SolvedAngles[2]);



		glm::mat4 T13 = glm::mat4(1);
		T13 = glm::translate(T13, glm::vec3(-302.0f, 630.0f, 0.0f));
		T13 = glm::rotate(T13, glm::radians(SolvedAngles[0]), glm::vec3(0.0f, 1.0f, 0.0f));
		//T13 = glm::translate(T13, glm::vec3(0.0f, 290.0f, 0.0f));
		T13 = glm::rotate(T13, glm::radians(SolvedAngles[1]), glm::vec3(0.0f, 0.0f, 1.0f));
		//T13 = glm::translate(T13, glm::vec3(0.0f, -290.0f, 0.0f));
		//T13 = glm::translate(T13, glm::vec3(0.0f, 560.0f, 0.0f));
		T13 = glm::rotate(T13, glm::radians(SolvedAngles[2]), glm::vec3(0.0f, 0.0f, 1.0f));
		//T13 = glm::translate(T13, glm::vec3(0.0f, -560.0f, 0.0f));

		glm::mat4 T16 = glm::mat4(1);
		T16 = glm::translate(T16, Pos);
		T16 = glm::rotate(T16, glm::radians(Eular.z), glm::vec3(0, 0, 1));
		T16 = glm::rotate(T16, glm::radians(Eular.y), glm::vec3(0, 1, 0));
		T16 = glm::rotate(T16, glm::radians(Eular.x), glm::vec3(1, 0, 0));

		glm::mat4 T35 = glm::inverse(T13) * T16;
		SolvedAngles[3] = atan2f(-T35[2].y, T35[2].z) / PI * 180.0f;
		SolvedAngles[4] = atan2f(-T35[1].x, T35[0].x) / PI * 180.0f;
		// 	ImGui::Text("Angle4 = %.3f", SolvedAngles[3]);
		// 	ImGui::Text("Angle5 = %.3f", SolvedAngles[4]);


		if (SolvedAngles[0] < 165.0f && SolvedAngles[0] > -165.0f)
		{
			Angle[m_Objectindex][0][m_index] = SolvedAngles[0];
		}
		else
		{
			return false;
		}
		if (SolvedAngles[1] < 110.0f && SolvedAngles[1] > -110.0f)
		{
			Angle[m_Objectindex][1][m_index] = SolvedAngles[1];
		}
		else
		{
			return false;
		}
		if (SolvedAngles[2] < 70.0f && SolvedAngles[2] > -110.0f)
		{
			Angle[m_Objectindex][2][m_index] = SolvedAngles[2];
		}
		else
		{
			return false;
		}
		if (SolvedAngles[3] < 160.0f && SolvedAngles[3] > -160.0f)
		{
			Angle[m_Objectindex][3][m_index] = SolvedAngles[3];
		}
		else
		{
			return false;
		}
		if (SolvedAngles[4] < 120.0f && SolvedAngles[4] > -120.0f)
		{
			Angle[m_Objectindex][4][m_index] = SolvedAngles[4];
		}
		else
		{
			return false;
		}
		//ChangeAngle();
		return true;

	}

	glm::vec3 Objects::GetAABBMaxPos()
	{
		return AABBMaxPos[m_Objectindex][m_index];
	}

	glm::vec3 Objects::GetAABBMinPos()
	{
		return AABBMinPos[m_Objectindex][m_index];
	}



	glm::vec3 Objects::GetScale()
	{
		return m_Scale[m_Objectindex];
	}

	glm::vec3 Objects::GetPos()
	{
		return m_Pos[m_Objectindex][m_index];
	}


	glm::vec3 Objects::GetRotate()
	{
		return m_Rotate[m_Objectindex][m_index];
	}

	void Objects::ChangePos(glm::vec3 ChangedPos)
	{
		m_Pos[m_Objectindex][m_index] += ChangedPos;
		for (int i = 0; i < m_model[m_Objectindex]->meshes.size(); i++)
		{
			//ModelMatrices[i][index] = glm::translate(ModelMatrices[i][index],ChangedPos/Scale);
			DefaultModelMatrices[m_Objectindex][i][m_index] = glm::rotate(DefaultModelMatrices[m_Objectindex][i][m_index], -m_Rotate[m_Objectindex][m_index].x, glm::vec3(1.0f, 0.0f, 0.0f));
			DefaultModelMatrices[m_Objectindex][i][m_index] = glm::rotate(DefaultModelMatrices[m_Objectindex][i][m_index], -m_Rotate[m_Objectindex][m_index].y, glm::vec3(0.0f, 1.0f, 0.0f));
			DefaultModelMatrices[m_Objectindex][i][m_index] = glm::rotate(DefaultModelMatrices[m_Objectindex][i][m_index], -m_Rotate[m_Objectindex][m_index].z, glm::vec3(0.0f, 0.0f, 1.0f));
			DefaultModelMatrices[m_Objectindex][i][m_index] = glm::translate(DefaultModelMatrices[m_Objectindex][i][m_index], ChangedPos / m_Scale[m_Objectindex]);
			DefaultModelMatrices[m_Objectindex][i][m_index] = glm::rotate(DefaultModelMatrices[m_Objectindex][i][m_index], m_Rotate[m_Objectindex][m_index].z, glm::vec3(0.0f, 0.0f, 1.0f));
			DefaultModelMatrices[m_Objectindex][i][m_index] = glm::rotate(DefaultModelMatrices[m_Objectindex][i][m_index], m_Rotate[m_Objectindex][m_index].y, glm::vec3(0.0f, 1.0f, 0.0f));
			DefaultModelMatrices[m_Objectindex][i][m_index] = glm::rotate(DefaultModelMatrices[m_Objectindex][i][m_index], m_Rotate[m_Objectindex][m_index].x, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if(haveAngle[m_Objectindex])
		{
			ChangeAngle();
		}
		SetAABB(m_Objectindex,m_index);
	}

	void Objects::ChangeRotate(glm::vec3 ChangedRotate, int RotateAxis)
	{

		m_Rotate[m_Objectindex][m_index] += ChangedRotate;
		for (int i = 0; i < m_model[m_Objectindex]->meshes.size(); i++)
		{
			//ModelMatrices[i][index] = glm::translate(ModelMatrices[i][index],ChangedPos/Scale);
			glm::vec3 RotateAxisVec3 = glm::vec3(0.0f);
			RotateAxisVec3[RotateAxis] = 1.0f;
			//DefaultModelMatrices[i][m_index] = glm::translate(DefaultModelMatrices[i][m_index], -m_Pos[m_index] / Scale);
			DefaultModelMatrices[m_Objectindex][i][m_index] = glm::rotate(DefaultModelMatrices[m_Objectindex][i][m_index], ChangedRotate[RotateAxis], RotateAxisVec3);
			//DefaultModelMatrices[i][m_index] = glm::translate(DefaultModelMatrices[i][m_index], m_Pos[m_index] / Scale);
		}
		if (haveAngle[m_Objectindex])
		{
			ChangeAngle();
		}
		SetAABB(m_Objectindex,m_index);
	}

	void Objects::SetChoosedIndex(int ObjectIndex, int index)
	{
		m_index = index;
		m_Objectindex = ObjectIndex;
	}

	bool Objects::CheckCollision(int ObjectIndex, int index, glm::vec3 CheckPos)
	{
		if (CheckPos.x > AABBMinPos[ObjectIndex][index].x&& CheckPos.x < AABBMaxPos[ObjectIndex][index].x)
		{
			if (CheckPos.y > AABBMinPos[ObjectIndex][index].y&& CheckPos.y < AABBMaxPos[ObjectIndex][index].y)
			{
				if (CheckPos.z > AABBMinPos[ObjectIndex][index].z&& CheckPos.z < AABBMaxPos[ObjectIndex][index].z)
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
		if (haveAngle[ObjectIndex])
		{
			vp = ModelMatrices[ObjectIndex][0][index] * glm::vec4(m_model[ObjectIndex]->meshes[0].vertices[0].Position, 1.0f);
		}
		else
		{
			vp = DefaultModelMatrices[ObjectIndex][0][index] * glm::vec4(m_model[ObjectIndex]->meshes[0].vertices[0].Position, 1.0f);

		}

		AABBMinPos[ObjectIndex][index] = glm::vec3(vp.x, vp.y, vp.z);
		AABBMaxPos[ObjectIndex][index] = glm::vec3(vp.x, vp.y, vp.z);


		for (int i = 0; i < m_model[ObjectIndex]->meshes.size(); i++)
		{
			for (int j = 0; j < m_model[ObjectIndex]->meshes[i].vertices.size(); j++)
			{
				glm::vec4 thisvp = glm::vec4(0.0f);
				if (haveAngle[ObjectIndex])
				{
					thisvp = ModelMatrices[ObjectIndex][i][index] * glm::vec4(m_model[ObjectIndex]->meshes[i].vertices[j].Position, 1.0f);
				}
				else
				{
					thisvp = DefaultModelMatrices[ObjectIndex][i][index] * glm::vec4(m_model[ObjectIndex]->meshes[i].vertices[j].Position, 1.0f);

				}
				if (thisvp.x < AABBMinPos[ObjectIndex][index].x)
				{
					AABBMinPos[ObjectIndex][index].x = thisvp.x;
				}
				if (thisvp.x > AABBMaxPos[ObjectIndex][index].x)
				{
					AABBMaxPos[ObjectIndex][index].x = thisvp.x;
				}
				if (thisvp.y < AABBMinPos[ObjectIndex][index].y)
				{
					AABBMinPos[ObjectIndex][index].y = thisvp.y;
				}
				if (thisvp.y > AABBMaxPos[ObjectIndex][index].y)
				{
					AABBMaxPos[ObjectIndex][index].y = thisvp.y;
				}
				if (thisvp.z < AABBMinPos[ObjectIndex][index].z)
				{
					AABBMinPos[ObjectIndex][index].z = thisvp.z;
				}
				if (thisvp.z > AABBMaxPos[ObjectIndex][index].z)
				{
					AABBMaxPos[ObjectIndex][index].z = thisvp.z;
				}
			}
		}
		
		
	}


	Hazel::Objects::ControlMode Objects::GetControlMode()
	{
		return controlmode[m_Objectindex];
	}

	int* Objects::GetControlModeAddress()
	{
		return (int*)&controlmode[m_Objectindex];
	}

	bool Objects::SaveScene()
	{

		//Objects
		std::fstream file("res/save/Objects.json", std::ios::out);

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
		for (auto it = ObjectsMap.begin(); it != ObjectsMap.end(); it++)
		{
			writer.StartObject();//层2
			writer.Key("index");
			writer.Int(i);
			writer.Key("name");
			writer.String((*it).c_str());
			writer.Key("amount");
			writer.Int(Amount[i]);
			writer.Key("filepath");
			writer.String(m_model[i]->m_path.c_str());
			writer.Key("haveangle");
			writer.Bool(haveAngle[i]);
			writer.Key("scale");
			writer.StartArray();
			writer.Double(m_Scale[i].x);
			writer.Double(m_Scale[i].y);
			writer.Double(m_Scale[i].z);
			writer.EndArray();
			writer.EndObject();
			i++;
		}
		writer.EndArray();
		writer.EndObject();
		file << strBuffer.GetString() << std::endl;



		//One Object
		std::fstream file1("res/save/ObjectsDetail.json", std::ios::out);

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
		for(int j = 0; j < ObjectAmount; j++)
		{
			
			writer1.StartObject();//层2
			writer1.Key(ObjectsMap[j].c_str());
			writer1.StartArray();//层3
			for (int i = 0; i<Amount[j]; i++)
			{
				writer1.StartObject();//层4
				writer1.Key("index");
				writer1.Int(i);
				writer1.Key("pos");
				writer1.StartArray();
				writer1.Double(m_Pos[j][i].x);
				writer1.Double(m_Pos[j][i].y);
				writer1.Double(m_Pos[j][i].z);
				writer1.EndArray();
				writer1.Key("rotate");
				writer1.StartArray();
				writer1.Double(m_Rotate[j][i].x);
				writer1.Double(m_Rotate[j][i].y);
				writer1.Double(m_Rotate[j][i].z);
				writer1.EndArray();
				
				writer1.Key("angle");
				writer1.StartArray();
				for (std::vector<float> m_angle : Angle[j])
				{
					writer1.Double(m_angle[i]);
				}
				writer1.EndArray();

				writer1.Key("poseular");
				writer1.StartArray();
				writer1.Double(Pos_Eular[j][i][0]);
				writer1.Double(Pos_Eular[j][i][1]);
				writer1.Double(Pos_Eular[j][i][2]);
				writer1.Double(Pos_Eular[j][i][3]);
				writer1.Double(Pos_Eular[j][i][4]);
				writer1.Double(Pos_Eular[j][i][5]);
				writer1.EndArray();
				
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

	bool Objects::LoadScene()
	{
		std::ifstream file("res/save/Objects.json", std::ios::in);
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
		
		std::ifstream file1("res/save/ObjectsDetail.json", std::ios::in);
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
						if (object1.HasMember(ObjectsMap[i].c_str()) && object1[ObjectsMap[i].c_str()].IsArray())
						{
							const rapidjson::Value& array3 = object1[ObjectsMap[i].c_str()];
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
									ChangeRotate(glm::vec3(0.0f, load_rotate.y, 0.0f), 1);
									if (object2.HasMember("angle") && object2["angle"].IsArray())
									{
										const rapidjson::Value& array6 = object2["angle"];

										size_t len3 = array6.Size();
										for (size_t k = 0; k < len3; k++)//每个关节
										{
											Angle[m_Objectindex][k][m_index] = array6[k].GetDouble();
										}
									}
									if (haveAngle[m_Objectindex])
									{
										ChangeAngle();
									}
									if (object2.HasMember("poseular") && object2["poseular"].IsArray())
									{
										const rapidjson::Value& array7 = object2["poseular"];
										Pos_Eular[i][j][0] = array7[0].GetDouble();
										Pos_Eular[i][j][1] = array7[1].GetDouble();
										Pos_Eular[i][j][2] = array7[2].GetDouble();
										Pos_Eular[i][j][3] = array7[3].GetDouble();
										Pos_Eular[i][j][4] = array7[4].GetDouble();
										Pos_Eular[i][j][5] = array7[5].GetDouble();

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


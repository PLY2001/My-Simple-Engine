#include "hzpch.h"
#include "Objects.h"

namespace Hazel {

	Objects::Objects()
	{

	}

	void Objects::AddObject(glm::vec3 Pos, glm::vec3 Rotation, glm::vec3 Scale, std::shared_ptr<Model>& model, bool hasAngle)
	{
		ObjectAmount++;
		m_Pos.resize(ObjectAmount);
		m_Pos[ObjectAmount-1].push_back(Pos);
		m_Rotate.resize(ObjectAmount);
		m_Rotate.back().push_back(Rotation);
		m_Scale.push_back(Scale);
		m_model.push_back(model);
		haveAngle.push_back(hasAngle);

		
		
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
		Angle.back().resize(model->meshes.size());
		for (int i = 0; i < model->meshes.size(); i++)
		{
			Angle[ObjectAmount-1][i].push_back(0);
		}
		
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
				ModelMatrices[ObjectIndex][i].back() = glm::scale(ModelMatrices[ObjectIndex][i].back(), m_Scale[ObjectIndex]);
			}
			DefaultModelMatrices[ObjectIndex][i].push_back(m_model[ObjectIndex]->mModelMatrix);
			DefaultModelMatrices[ObjectIndex][i].back() = glm::scale(DefaultModelMatrices[ObjectIndex][i].back(), m_Scale[ObjectIndex]);
		}


		AABBMinPos[ObjectIndex].push_back(m_Pos[ObjectIndex][0]);
		AABBMaxPos[ObjectIndex].push_back(m_Pos[ObjectIndex][0]);
		SetAABB(ObjectIndex,0);
		
	}



	void Objects::AddAmount()
	{
		//增加模型
		Amount[m_Objectindex]++;
		if (int increase = Amount[m_Objectindex] - DefaultModelMatrices[m_Objectindex][0].size() > 0)
		{
			for (int j = 0; j < increase; j++)
			{
				m_Pos[m_Objectindex].push_back(glm::vec3((Amount[m_Objectindex] - 1) * 10.0f, 0.0f, 0.0f));
				m_Rotate[m_Objectindex].push_back(glm::vec3(0.0f, 0.0f, 0.0f));
			}
			for (int i = 0; i < m_model[m_Objectindex]->meshes.size(); i++)
			{
				for (int j = 0; j < increase; j++)
				{
					if (haveAngle[m_Objectindex])
					{
						ModelMatrices[m_Objectindex][i].push_back(ModelMatrix(glm::vec3(m_Pos[m_Objectindex].back())).matrix);
						ModelMatrices[m_Objectindex][i].back() = glm::scale(ModelMatrices[m_Objectindex][i].back(), m_Scale[m_Objectindex]);
					}
					DefaultModelMatrices[m_Objectindex][i].push_back(ModelMatrix(glm::vec3(m_Pos[m_Objectindex].back())).matrix);
					DefaultModelMatrices[m_Objectindex][i].back() = glm::scale(DefaultModelMatrices[m_Objectindex][i].back(), m_Scale[m_Objectindex]);
				}
			}

		}
		AABBMinPos[m_Objectindex].push_back(m_Pos[m_Objectindex][Amount[m_Objectindex] - 1]);
		AABBMaxPos[m_Objectindex].push_back(m_Pos[m_Objectindex][Amount[m_Objectindex] - 1]);
		
		for (int i = 0; i < m_model[m_Objectindex]->meshes.size(); i++)
		{
			Angle[m_Objectindex][i].push_back(0);
		}
		
		SetAABB(m_Objectindex,Amount[m_Objectindex] - 1);
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

	bool Objects::SolveAngle(glm::vec3 Pos, glm::vec3 Eular)
	{
		glm::mat4 backwardTranslate = glm::mat4(1);;
		backwardTranslate = glm::translate(backwardTranslate, glm::vec3(72.0f, 0.0f, 0.0f));


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

}


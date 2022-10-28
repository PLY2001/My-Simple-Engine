#include "hzpch.h"
#include "ABBIRB120.h"

Hazel::ABBIRB120::ABBIRB120(glm::vec3 Pos, glm::vec3 Rotation, glm::vec3 Scale,std::shared_ptr<Model>& model):Rotation(Rotation),Scale(Scale),model(model)
{
	m_Pos.push_back(Pos);
	Angle.resize(6);
	for (int i = 0; i < 6; i++)
	{
		Angle[i].push_back(0);
	}
	
	
}

void Hazel::ABBIRB120::InitModelMatrices()
{
	//创建变换矩阵
	ModelMatrices.resize(model->meshes.size());
	DefaultModelMatrices.resize(model->meshes.size());
	for (int i = 0; i < model->meshes.size(); i++)
	{
		ModelMatrices[i].push_back(model->mModelMatrix);
		ModelMatrices[i].back() = glm::scale(ModelMatrices[i].back(), Scale);
		DefaultModelMatrices[i].push_back(model->mModelMatrix);
		DefaultModelMatrices[i].back() = glm::scale(DefaultModelMatrices[i].back(),Scale);
	}

	AABBMinPos.push_back(m_Pos[0]);
	AABBMaxPos.push_back(m_Pos[0]);
	SetAABB(0);
}



void Hazel::ABBIRB120::AddAmount()
{
	//增加模型
	Amount++;
	if (int increase = Amount - ModelMatrices[0].size() > 0)
	{
		for (int j = 0; j < increase; j++)
		{
			m_Pos.push_back(glm::vec3((Amount - 1) * 10.0f, 0.0f, 0.0f));
		}
		for (int i = 0; i < model->meshes.size(); i++)
		{
			for (int j = 0; j < increase; j++)
			{
				ModelMatrices[i].push_back(ModelMatrix(glm::vec3(m_Pos.back())).matrix);
				ModelMatrices[i].back() = glm::scale(ModelMatrices[i].back(), Scale);
				DefaultModelMatrices[i].push_back(ModelMatrix(glm::vec3(m_Pos.back())).matrix);
				DefaultModelMatrices[i].back() = glm::scale(DefaultModelMatrices[i].back(), Scale);
			}
		}
		
	}
	AABBMinPos.push_back(m_Pos[Amount-1]);
	AABBMaxPos.push_back(m_Pos[Amount - 1]);
	for (int i = 0; i < 6; i++)
	{
		Angle[i].push_back(0);
	}
	SetAABB(Amount-1);
}

int Hazel::ABBIRB120::GetAmount()
{
	return Amount;
}

void Hazel::ABBIRB120::ChangeAngle(int index)
{
	//归位
	for (int j = 1; j < model->meshes.size(); j++)
	{
// 		for (int i = 0; i < Amount; i++)
// 		{
 			ModelMatrices[j][index] = DefaultModelMatrices[j][index];
// 		}

	}


	for (int j = 1; j < model->meshes.size(); j++)
	{
// 		for (int i = 0; i < Amount; i++)
// 		{
			if (j > 0)
			{
				ModelMatrices[j][index] = glm::rotate(ModelMatrices[j][index], glm::radians(Angle[0][index]), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (j > 1)
			{

				ModelMatrices[j][index] = glm::translate(ModelMatrices[j][index], glm::vec3(0.0f, 290.0f, 0.0f));
				ModelMatrices[j][index] = glm::rotate(ModelMatrices[j][index], glm::radians(Angle[1][index]), glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrices[j][index] = glm::translate(ModelMatrices[j][index], glm::vec3(0.0f, -290.0f, 0.0f));

			}
			if (j > 2)
			{
				ModelMatrices[j][index] = glm::translate(ModelMatrices[j][index], glm::vec3(0.0f, 560.0f, 0.0f));
				ModelMatrices[j][index] = glm::rotate(ModelMatrices[j][index], glm::radians(Angle[2][index]), glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrices[j][index] = glm::translate(ModelMatrices[j][index], glm::vec3(0.0f, -560.0f, 0.0f));
			}
			if (j > 3)
			{
				ModelMatrices[j][index] = glm::translate(ModelMatrices[j][index], glm::vec3(0.0f, 630.0f, 0.0f));
				ModelMatrices[j][index] = glm::rotate(ModelMatrices[j][index], glm::radians(Angle[3][index]), glm::vec3(1.0f, 0.0f, 0.0f));
				ModelMatrices[j][index] = glm::translate(ModelMatrices[j][index], glm::vec3(0.0f, -630.0f, 0.0f));
			}
			if (j > 4)
			{
				ModelMatrices[j][index] = glm::translate(ModelMatrices[j][index], glm::vec3(-302.0f, 630.0f, 0.0f));
				ModelMatrices[j][index] = glm::rotate(ModelMatrices[j][index], glm::radians(Angle[4][index]), glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrices[j][index] = glm::translate(ModelMatrices[j][index], glm::vec3(302.0f, -630.0f, 0.0f));
			}
			if (j > 5)
			{
				ModelMatrices[j][index] = glm::translate(ModelMatrices[j][index], glm::vec3(-302.0f, 630.0f, 0.0f));
				ModelMatrices[j][index] = glm::rotate(ModelMatrices[j][index], glm::radians(Angle[5][index]), glm::vec3(1.0f, 0.0f, 0.0f));
				ModelMatrices[j][index] = glm::translate(ModelMatrices[j][index], glm::vec3(302.0f, -630.0f, 0.0f));
			}

		//}

	}
	SetAABB(index);
	
}

float* Hazel::ABBIRB120::SetAngle(int index,int Axis)
{
	return &Angle[Axis - 1][index];
}

bool Hazel::ABBIRB120::SolveAngle(int index,glm::vec3 Pos, glm::vec3 Eular)
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
		Angle[0][index] = SolvedAngles[0];
	}
	else
	{
		return false;
	}
	if (SolvedAngles[1] < 110.0f && SolvedAngles[1] > -110.0f)
	{
		Angle[1][index] = SolvedAngles[1];
	}
	else
	{
		return false;
	}
	if (SolvedAngles[2] < 70.0f && SolvedAngles[2] > -110.0f)
	{
		Angle[2][index] = SolvedAngles[2];
	}
	else
	{
		return false;
	}
	if (SolvedAngles[3] < 160.0f && SolvedAngles[3] > -160.0f)
	{
		Angle[3][index] = SolvedAngles[3];
	}
	else
	{
		return false;
	}
	if (SolvedAngles[4] < 120.0f && SolvedAngles[4] > -120.0f)
	{
		Angle[4][index] = SolvedAngles[4];
	}
	else
	{
		return false;
	}

	return true;

}

glm::vec3 Hazel::ABBIRB120::GetAABBMaxPos(int index)
{
	return AABBMaxPos[index];
}

glm::vec3 Hazel::ABBIRB120::GetAABBMinPos(int index)
{
	return AABBMinPos[index];
}



glm::vec3 Hazel::ABBIRB120::GetScale()
{
	return Scale;
}

glm::vec3 Hazel::ABBIRB120::GetPos(int index)
{
	return m_Pos[index];
}

void Hazel::ABBIRB120::ChangePos(int index,glm::vec3 ChangedPos)
{
	m_Pos[index] += ChangedPos;
	for (int i = 0; i < model->meshes.size(); i++)
	{
		ModelMatrices[i][index] = glm::translate(ModelMatrices[i][index],ChangedPos/Scale);
		DefaultModelMatrices[i][index] = glm::translate(DefaultModelMatrices[i][index], ChangedPos/Scale);
	}
	ChangeAngle(index);
	SetAABB(index);
}

void Hazel::ABBIRB120::SetAABB(int index)
{
// 	for(int n = 0; n < Amount; n++)
// 	{
		glm::vec4 vp = ModelMatrices[0][index] * glm::vec4(model->meshes[0].vertices[0].Position, 1.0f);

		AABBMinPos[index] = glm::vec3(vp.x, vp.y, vp.z);
		AABBMaxPos[index] = glm::vec3(vp.x, vp.y, vp.z);


		for (int i = 0; i < model->meshes.size(); i++)
		{
			for (int j = 0; j < model->meshes[i].vertices.size(); j++)
			{
				glm::vec4 thisvp = ModelMatrices[i][index] * glm::vec4(model->meshes[i].vertices[j].Position, 1.0f);
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


#include "hzpch.h"
#include "ABBIRB120.h"

Hazel::ABBIRB120::ABBIRB120(glm::vec3 Pos, glm::vec3 Rotation, std::shared_ptr<Model>& model):Pos(Pos),Rotation(Rotation),model(model)
{

}

void Hazel::ABBIRB120::InitModelMatrices()
{
	//创建变换矩阵
	ModelMatrices.resize(model->meshes.size());
	DefaultModelMatrices.resize(model->meshes.size());
	for (int i = 0; i < model->meshes.size(); i++)
	{
		ModelMatrices[i].push_back(ModelMatrix(Pos).Matrix);
		ModelMatrices[i].back() = glm::scale(ModelMatrices[i].back(), glm::vec3(0.01f, 0.01f, 0.01f));
		DefaultModelMatrices[i].push_back(ModelMatrix(Pos).Matrix);
		DefaultModelMatrices[i].back() = glm::scale(DefaultModelMatrices[i].back(), glm::vec3(0.01f, 0.01f, 0.01f));
	}
}



void Hazel::ABBIRB120::AddAmount()
{
	//增加模型
	Amount++;
	if (int increase = Amount - ModelMatrices[0].size() > 0)
	{
		for (int i = 0; i < model->meshes.size(); i++)
		{
			for (int j = 0; j < increase; j++)
			{
				ModelMatrices[i].push_back(ModelMatrix(glm::vec3((Amount - 1) * 10.0f, 0.0f, 0.0f)).Matrix);
				ModelMatrices[i].back() = glm::scale(ModelMatrices[i].back(), glm::vec3(0.01f, 0.01f, 0.01f));
				DefaultModelMatrices[i].push_back(ModelMatrix(glm::vec3((Amount - 1) * 10.0f, 0.0f, 0.0f)).Matrix);
				DefaultModelMatrices[i].back() = glm::scale(DefaultModelMatrices[i].back(), glm::vec3(0.01f, 0.01f, 0.01f));
			}
		}

	}
}

int Hazel::ABBIRB120::GetAmount()
{
	return Amount;
}

void Hazel::ABBIRB120::ChangeAngle()
{
	//归位
	for (int j = 1; j < model->meshes.size(); j++)
	{
		for (int i = 0; i < Amount; i++)
		{
			ModelMatrices[j][i] = DefaultModelMatrices[j][i];
		}

	}


	for (int j = 1; j < model->meshes.size(); j++)
	{
		for (int i = 0; i < Amount; i++)
		{
			if (j > 0)
			{
				ModelMatrices[j][i] = glm::rotate(ModelMatrices[j][i], glm::radians(Angle[0]), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (j > 1)
			{

				ModelMatrices[j][i] = glm::translate(ModelMatrices[j][i], glm::vec3(0.0f, 290.0f, 0.0f));
				ModelMatrices[j][i] = glm::rotate(ModelMatrices[j][i], glm::radians(Angle[1]), glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrices[j][i] = glm::translate(ModelMatrices[j][i], glm::vec3(0.0f, -290.0f, 0.0f));

			}
			if (j > 2)
			{
				ModelMatrices[j][i] = glm::translate(ModelMatrices[j][i], glm::vec3(0.0f, 560.0f, 0.0f));
				ModelMatrices[j][i] = glm::rotate(ModelMatrices[j][i], glm::radians(Angle[2]), glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrices[j][i] = glm::translate(ModelMatrices[j][i], glm::vec3(0.0f, -560.0f, 0.0f));
			}
			if (j > 3)
			{
				ModelMatrices[j][i] = glm::translate(ModelMatrices[j][i], glm::vec3(0.0f, 630.0f, 0.0f));
				ModelMatrices[j][i] = glm::rotate(ModelMatrices[j][i], glm::radians(Angle[3]), glm::vec3(1.0f, 0.0f, 0.0f));
				ModelMatrices[j][i] = glm::translate(ModelMatrices[j][i], glm::vec3(0.0f, -630.0f, 0.0f));
			}
			if (j > 4)
			{
				ModelMatrices[j][i] = glm::translate(ModelMatrices[j][i], glm::vec3(-302.0f, 630.0f, 0.0f));
				ModelMatrices[j][i] = glm::rotate(ModelMatrices[j][i], glm::radians(Angle[4]), glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrices[j][i] = glm::translate(ModelMatrices[j][i], glm::vec3(302.0f, -630.0f, 0.0f));
			}
			if (j > 5)
			{
				ModelMatrices[j][i] = glm::translate(ModelMatrices[j][i], glm::vec3(-302.0f, 630.0f, 0.0f));
				ModelMatrices[j][i] = glm::rotate(ModelMatrices[j][i], glm::radians(Angle[5]), glm::vec3(1.0f, 0.0f, 0.0f));
				ModelMatrices[j][i] = glm::translate(ModelMatrices[j][i], glm::vec3(302.0f, -630.0f, 0.0f));
			}

		}

	}
}

float* Hazel::ABBIRB120::SetAngle(int Axis)
{
	return &Angle[Axis - 1];
}


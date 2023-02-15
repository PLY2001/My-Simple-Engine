#include "hzpch.h"
#include "Arrow.h"
#include "Application.h"

namespace Hazel{

	Arrow::Arrow(glm::vec3 Pos, glm::vec3 Rotation, glm::vec3 Scale, std::shared_ptr<Model>& model) :Pos(Pos), Rotation(Rotation), Scale(Scale), model(model)
	{
		
		ModelMatrix = glm::mat4(1);
		ModelMatrix = glm::translate(ModelMatrix, Pos);
		ModelMatrix = glm::scale(ModelMatrix,Scale);
		for (int i = 0; i < 3; i++)
		{
			AABBMinPos.push_back(Pos);
			AABBMaxPos.push_back(Pos);
		}
		SetAABB();
	}

	void Arrow::ChangePos(glm::vec3 ChangedPos)
	{

		Pos = ChangedPos;
		SetAABB();
	}

	void Arrow::ChangeRotate(glm::vec3 ChangedRotate)
	{
		Rotation = ChangedRotate;
		SetAABB();
	}

	glm::mat4 Arrow::GetModelMatrix()
	{
 		ModelMatrix = glm::mat4(1.0f);
 		
		
		ModelMatrix = glm::translate(ModelMatrix, Pos);
		ModelMatrix = glm::scale(ModelMatrix, Scale);
		
		
		glm::qua<float> Quaternion = glm::qua<float>(Rotation);
		glm::mat4 RotateMatrix = glm::mat4(1.0f);
		RotateMatrix = glm::mat4_cast(Quaternion) * RotateMatrix;
		ModelMatrix = ModelMatrix*RotateMatrix;
		
	
		
		

		
 	

		return ModelMatrix;
	}

	glm::vec3 Arrow::GetAABBMaxPos(int axis)
	{
		return AABBMaxPos[axis];
	}

	glm::vec3 Arrow::GetAABBMinPos(int axis)
	{
		return AABBMinPos[axis];
	}

	void Arrow::SetAABB()
	{

		for (int n = 0; n < 3; n++)
		{
			glm::vec4 vp = ModelMatrix * glm::vec4(model->meshes[n].vertices[0].Position, 1.0f);

			AABBMinPos[n] = glm::vec3(vp.x, vp.y, vp.z);
			AABBMaxPos[n] = glm::vec3(vp.x, vp.y, vp.z);

			for (int j = 0; j < model->meshes[n].vertices.size(); j++)
			{
				glm::vec4 thisvp = ModelMatrix * glm::vec4(model->meshes[n].vertices[j].Position, 1.0f);
				if (thisvp.x < AABBMinPos[n].x)
				{
					AABBMinPos[n].x = thisvp.x;
				}
				if (thisvp.x > AABBMaxPos[n].x)
				{
					AABBMaxPos[n].x = thisvp.x;
				}
				if (thisvp.y < AABBMinPos[n].y)
				{
					AABBMinPos[n].y = thisvp.y;
				}
				if (thisvp.y > AABBMaxPos[n].y)
				{
					AABBMaxPos[n].y = thisvp.y;
				}
				if (thisvp.z < AABBMinPos[n].z)
				{
					AABBMinPos[n].z = thisvp.z;
				}
				if (thisvp.z > AABBMaxPos[n].z)
				{
					AABBMaxPos[n].z = thisvp.z;
				}
			}

		}

	}
}


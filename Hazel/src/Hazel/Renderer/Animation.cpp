#include "hzpch.h"
#include "Animation.h"

namespace Hazel {


	Animation::Animation(bool HasAngle):HaveAngle(HasAngle)
	{

	}

	void Animation::Reset()
	{
		frame = 0;
		TimeNow = 0.0f;
		Path_index = 0;
		Playing = false;
	}

	void Animation::SetPathPos(glm::vec3 Pos)
	{
		
		if (m_Path_Pos.size() > 0)
		{
			float mod = (m_Path_Pos.back().x - Pos.x) * (m_Path_Pos.back().x - Pos.x) + (m_Path_Pos.back().y - Pos.y) * (m_Path_Pos.back().y - Pos.y) + (m_Path_Pos.back().z - Pos.z) * (m_Path_Pos.back().z - Pos.z);
			Path_Pos_Mod.push_back(glm::sqrt(mod));
		}
		m_Path_Pos.push_back(Pos);
	}

	void Animation::SetPathRotate(glm::vec3 Rotate)
	{
		if (m_Path_Rotate.size() > 0)
		{
			Path_Rotate_Mod.push_back(abs(m_Path_Rotate.back().y - Rotate.y)*10.0f);
		}
		m_Path_Rotate.push_back(Rotate);
	}



	PathPoint Animation::GetPathPoint(float TotalTime, float deltaTime)
	{
		Path_Pos_Last = Path_Pos_Now;
		Path_Rotate_Last = Path_Rotate_Now;
		if (frame == 0)
		{
			Path_Pos_Last = m_Path_Pos.front();
			Path_Rotate_Last = m_Path_Rotate.front();
		}
		float thisTotalTime = TotalTime * (Path_Pos_Mod[Path_index] + Path_Rotate_Mod[Path_index]) / (accumulate(Path_Pos_Mod.begin(), Path_Pos_Mod.end(), 0)+ accumulate(Path_Rotate_Mod.begin(), Path_Rotate_Mod.end(), 0));
		if ((deltaTime / (thisTotalTime - TimeNow)) < 0.99999f)//未播放完毕
		{
			Path_Pos_Now = Path_Pos_Last + (m_Path_Pos[Path_index+1] - Path_Pos_Last) * deltaTime / (thisTotalTime - TimeNow);
			Path_Rotate_Now = Path_Rotate_Last + (m_Path_Rotate[Path_index + 1] - Path_Rotate_Last) * deltaTime / (thisTotalTime - TimeNow);
			TimeNow += deltaTime;
		}
		else//播放完毕
		{
			Path_Pos_Now = m_Path_Pos[Path_index + 1];
			Path_Rotate_Now = m_Path_Rotate[Path_index + 1];
			Path_index++;
			TimeNow = 0.0f;
			if (Path_index + 1 > Path_Pos_Mod.size())
			{
				Playing = false;
			}
		}

		
		frame++;
		pathpoint.Path_Pos = Path_Pos_Now - Path_Pos_Last;
		pathpoint.Path_Rotate = Path_Rotate_Now - Path_Rotate_Last;

		return pathpoint;
	}

	glm::vec3 Animation::GetPathKeyPos(int index)
	{
		return m_Path_Pos[index];
	}

	glm::vec3 Animation::GetPathKeyRotate(int index)
	{
		return m_Path_Rotate[index];
	}

}
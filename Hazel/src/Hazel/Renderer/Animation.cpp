#include "hzpch.h"
#include "Animation.h"

namespace Hazel {


	Animation::Animation(bool HasAngle):HaveAngle(HasAngle)
	{

	}

	void Animation::Reset()
	{
		TotalTime = accumulate(Path_Time.begin(), Path_Time.end(), 0);
		frame = 0;
		TimeNow = 0.0f;
		Path_index = 0;
		Playing = false;
	}

	void Animation::SetPathPos(glm::vec3 Pos)
	{
		m_Path_Pos.push_back(Pos);
	}

	void Animation::SetPathRotate(glm::vec3 Rotate)
	{
		m_Path_Rotate.push_back(Rotate);
	}

	void Animation::SetPathHandPos(glm::vec3 HandPos)
	{
		m_Path_HandPos.push_back(HandPos);
	}

	void Animation::SetPathHandEular(glm::vec3 HandEular)
	{
		m_Path_HandEular.push_back(HandEular);
	}

	PathPoint Animation::GetPathPoint(float deltaTime)
	{

		float thisTotalTime = Path_Time[Path_index];
		Path_Pos_Last = Path_Pos_Now;
		Path_Rotate_Last = Path_Rotate_Now;
		if (HaveAngle)
		{
			Path_HandPos_Last = Path_HandPos_Now;
			Path_HandEular_Last = Path_HandEular_Now;
		}
		if (frame == 0)
		{
			Path_Pos_Last = m_Path_Pos.front();
			Path_Rotate_Last = m_Path_Rotate.front();
			if (HaveAngle)
			{
				Path_HandPos_Last = m_Path_HandPos.front();
				Path_HandEular_Last = m_Path_HandEular.front();
			}
		}

		
		if ((deltaTime / (thisTotalTime - TimeNow)) < 0.99999f)//未播放完毕
		{
			Path_Pos_Now = Path_Pos_Last + (m_Path_Pos[Path_index+1] - Path_Pos_Last) * deltaTime / (thisTotalTime - TimeNow);
			Path_Rotate_Now = Path_Rotate_Last + (m_Path_Rotate[Path_index + 1] - Path_Rotate_Last) * deltaTime / (thisTotalTime - TimeNow);
			if (HaveAngle)
			{
				Path_HandPos_Now = Path_HandPos_Last + (m_Path_HandPos[Path_index + 1] - Path_HandPos_Last) * deltaTime / (thisTotalTime - TimeNow);
				Path_HandEular_Now = Path_HandEular_Last + (m_Path_HandEular[Path_index + 1] - Path_HandEular_Last) * deltaTime / (thisTotalTime - TimeNow);
			}
			TimeNow += deltaTime;
		}
		else//播放完毕
		{
			Path_Pos_Now = m_Path_Pos[Path_index + 1];
			Path_Rotate_Now = m_Path_Rotate[Path_index + 1];
			if (HaveAngle)
			{
				Path_HandPos_Now = m_Path_HandPos[Path_index + 1];
				Path_HandEular_Now = m_Path_HandEular[Path_index + 1];
			}
			Path_index++;
			TimeNow = 0.0f;
			if (Path_index + 1 > Path_Time.size())
			{
				Playing = false;
			}
		}

		
		frame++;
		pathpoint.Path_Pos = Path_Pos_Now - Path_Pos_Last;
		pathpoint.Path_Rotate = Path_Rotate_Now - Path_Rotate_Last;
		pathpoint.Path_HandPos = Path_HandPos_Now;
		pathpoint.Path_HandEular = Path_HandEular_Now;

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

	glm::vec3 Animation::GetPathKeyHandPos(int index)
	{
		return m_Path_HandPos[index];
	}

	glm::vec3 Animation::GetPathKeyHandEular(int index)
	{
		return m_Path_HandEular[index];
	}

	void Animation::SetPathTime(float Time)
	{
		if (m_Path_Pos.size() > 1)
		{
			Path_Time.push_back(Time);
		}
		HaveAnimation = true;
	}

}
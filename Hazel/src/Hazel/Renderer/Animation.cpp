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
		TotalTimeNow = 0.0f;
		Path_index = 0;
		Playing = false;
	}

	void Animation::SetPathPos(glm::vec3 Pos)
	{
		m_Path_Pos.push_back(Pos);
	}

	void Animation::SetPathRotate(glm::qua<float> RotateQuaternion)
	{
		m_Path_Rotate.push_back(RotateQuaternion);
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
			if(pathmodelist[Path_index] == PathMode::Straght)
			{
				Path_Pos_Now = Path_Pos_Last + (m_Path_Pos[Path_index + 1] - Path_Pos_Last) * deltaTime / (thisTotalTime - TimeNow);
				Path_Rotate_Now = glm::slerp(Path_Rotate_Last, m_Path_Rotate[Path_index + 1], deltaTime / (thisTotalTime - TimeNow));
				
				if (HaveAngle)
				{
					Path_HandPos_Now = Path_HandPos_Last + (m_Path_HandPos[Path_index + 1] - Path_HandPos_Last) * deltaTime / (thisTotalTime - TimeNow);
					Path_HandEular_Now = Path_HandEular_Last + (m_Path_HandEular[Path_index + 1] - Path_HandEular_Last) * deltaTime / (thisTotalTime - TimeNow);
				}
			}
			if (pathmodelist[Path_index] == PathMode::Circle)
			{
				float theata = acos(glm::dot(glm::normalize(glm::vec3(m_Path_Pos[Path_index].x-CircleCenterList[Path_index].x,0.0f, m_Path_Pos[Path_index].z - CircleCenterList[Path_index].z)), glm::normalize(glm::vec3(m_Path_Pos[Path_index+1].x - CircleCenterList[Path_index].x,0.0f, m_Path_Pos[Path_index + 1].z - CircleCenterList[Path_index].z))));
				glm::vec4 TempPos = glm::vec4(Path_Pos_Last.x - CircleCenterList[Path_index].x,0.0f, Path_Pos_Last.z - CircleCenterList[Path_index].z,1.0f);
				glm::qua<float> Quaternion = glm::qua<float>(glm::vec3(0.0f,theata * deltaTime / (thisTotalTime),0.0f));
				glm::mat4 RotateMatrix = glm::mat4(1.0f);
				RotateMatrix = glm::mat4_cast(Quaternion) * RotateMatrix;
				TempPos = RotateMatrix * TempPos;
				Path_Pos_Now = glm::vec3(TempPos.x, Path_Pos_Last.y + (m_Path_Pos[Path_index + 1].y - Path_Pos_Last.y) * deltaTime / (thisTotalTime - TimeNow), TempPos.z) + glm::vec3(CircleCenterList[Path_index].x,0.0f, CircleCenterList[Path_index].z);
				
// 				float d1 = sqrt((m_Path_Pos[Path_index + 1].x - m_Path_Pos[Path_index].x) * (m_Path_Pos[Path_index + 1].x - m_Path_Pos[Path_index].x) + (m_Path_Pos[Path_index + 1].z - m_Path_Pos[Path_index].z) * (m_Path_Pos[Path_index + 1].z - m_Path_Pos[Path_index].z)) / 2.0f / sin(theata / 2.0);
// 				float d2 = 2.0 * d1 * sin(TimeNow * theata / 2.0 / thisTotalTime);
// 				float d3 = 2.0 * d1 * sin((thisTotalTime - TimeNow) * theata / 2.0 / thisTotalTime);
// 
// 				float x1 = CircleCenterList[Path_index].x;
// 				float y1 = CircleCenterList[Path_index].z;
// 				float x2 = m_Path_Pos[Path_index].x;
// 				float y2 = m_Path_Pos[Path_index].z;
// 				float x3 = m_Path_Pos[Path_index+1].x;
// 				float y3 = m_Path_Pos[Path_index+1].z;
// 				
// 				float a11 = 2 * (x1 - x3);
// 				float a12 = 2 * (y1 - y3);
// 				float b1 = pow(x1, 2) - pow(x3, 2)+ pow(y1, 2) - pow(y3, 2)+ pow(d3, 2) - pow(d1, 2);
// 				float a21 = 2 * (x2 - x3);
// 				float a22 = 2 * (y2 - y3);
// 				float b2 = pow(x2, 2) - pow(x3, 2)+ pow(y2, 2) - pow(y3, 2)+ pow(d3, 2) - pow(d2, 2);
// 				if ((a11 * a22 - a12 * a21) < 0.0001f && (a11 * a22 - a12 * a21) > -0.0001f)
// 				{
// 					Path_Pos_Now = Path_Pos_Last + (m_Path_Pos[Path_index + 1] - Path_Pos_Last) * deltaTime / (thisTotalTime - TimeNow);
// 				}
// 				else
// 				{
// 					Path_Pos_Now = glm::vec3((b1 * a22 - a12 * b2) / (a11 * a22 - a12 * a21), Path_Pos_Last.y, (a11 * b2 - b1 * a21) / (a11 * a22 - a12 * a21));
// 				}
				
				//Path_Rotate_Now = Path_Rotate_Last + (m_Path_Rotate[Path_index + 1] - Path_Rotate_Last) * deltaTime / (thisTotalTime - TimeNow);
				Path_Rotate_Now = glm::slerp(Path_Rotate_Last, m_Path_Rotate[Path_index + 1], deltaTime / (thisTotalTime-TimeNow));
				
				
				if (HaveAngle)
				{
					float theata1 = acos(glm::dot(glm::normalize(glm::vec3(m_Path_HandPos[Path_index].x - CircleCenterList[Path_index].x, 0.0f, m_Path_HandPos[Path_index].z - CircleCenterList[Path_index].z)), glm::normalize(glm::vec3(m_Path_HandPos[Path_index + 1].x - CircleCenterList[Path_index].x, 0.0f, m_Path_HandPos[Path_index + 1].z - CircleCenterList[Path_index].z))));
					glm::vec4 TempHandPos = glm::vec4(Path_HandPos_Last.x - CircleCenterList[Path_index].x, 0.0f, Path_HandPos_Last.z - CircleCenterList[Path_index].z, 1.0f);
					glm::qua<float> Quaternion1 = glm::qua<float>(glm::vec3(0.0f, theata1 * deltaTime / (thisTotalTime), 0.0f));
					glm::mat4 RotateMatrix1 = glm::mat4(1.0f);
					RotateMatrix1 = glm::mat4_cast(Quaternion1) * RotateMatrix1;
					TempHandPos = RotateMatrix1 * TempHandPos;
					Path_HandPos_Now = glm::vec3(TempHandPos.x, Path_HandPos_Last.y + (m_Path_HandPos[Path_index + 1].y - Path_HandPos_Last.y) * deltaTime / (thisTotalTime - TimeNow), TempHandPos.z) + glm::vec3(CircleCenterList[Path_index].x, 0.0f, CircleCenterList[Path_index].z);
// 					float d11 = sqrt((m_Path_HandPos[Path_index + 1].x - m_Path_HandPos[Path_index].x) * (m_Path_HandPos[Path_index + 1].x - m_Path_HandPos[Path_index].x) + (m_Path_HandPos[Path_index + 1].z - m_Path_HandPos[Path_index].z) * (m_Path_HandPos[Path_index + 1].z - m_Path_HandPos[Path_index].z)) / 2.0f / sin(theata1 / 2.0);
// 					float d21 = 2.0 * d11 * sin(TimeNow * theata1 / 2.0 / thisTotalTime);
// 					float d31 = 2.0 * d11 * sin((thisTotalTime - TimeNow) * theata1 / 2.0 / thisTotalTime);
// 
// 					float x11 = CircleCenterList[Path_index].x;
// 					float y11 = CircleCenterList[Path_index].z;
// 					float x21 = m_Path_HandPos[Path_index].x;
// 					float y21 = m_Path_HandPos[Path_index].z;
// 					float x31 = m_Path_HandPos[Path_index + 1].x;
// 					float y31 = m_Path_HandPos[Path_index + 1].z;
// 
// 					float a111 = 2 * (x11 - x31);
// 					float a121 = 2 * (y11 - y31);
// 					float b11 = pow(x11, 2) - pow(x31, 2) + pow(y11, 2) - pow(y31, 2) + pow(d31, 2) - pow(d11, 2);
// 					float a211 = 2 * (x21 - x31);
// 					float a221 = 2 * (y21 - y31);
// 					float b21 = pow(x21, 2) - pow(x31, 2) + pow(y21, 2) - pow(y31, 2) + pow(d31, 2) - pow(d21, 2);
// 
// 					if ((a111 * a221 - a121 * a211) < 0.0001f && (a111 * a221 - a121 * a211) > -0.0001f)
// 					{
// 						Path_HandPos_Now = Path_HandPos_Last + (m_Path_HandPos[Path_index + 1] - Path_HandPos_Last) * deltaTime / (thisTotalTime - TimeNow);
// 					}
// 					else
// 					{
// 						Path_HandPos_Now = glm::vec3((b11 * a221 - a121 * b21) / (a111 * a221 - a121 * a211), Path_HandPos_Last.y, (a111 * b21 - b11 * a211) / (a111 * a221 - a121 * a211));
// 					}

					
					Path_HandEular_Now = Path_HandEular_Last + (m_Path_HandEular[Path_index + 1] - Path_HandEular_Last) * deltaTime / (thisTotalTime - TimeNow);
				}
			}
			TimeNow += deltaTime;
			TotalTimeNow += deltaTime;
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
			TimeNow = deltaTime + TimeNow - thisTotalTime;
			if (Path_index + 1 > Path_Time.size())
			{
				Playing = false;
			}
		}

		
		frame++;
		pathpoint.Path_Pos = Path_Pos_Now - Path_Pos_Last;
		pathpoint.Path_Rotate = Path_Rotate_Now;// - Path_Rotate_Last;
		pathpoint.Path_HandPos = Path_HandPos_Now;
		pathpoint.Path_HandEular = Path_HandEular_Now;
		pathpoint.Path_State2 = m_Path_State2[Path_index];

		return pathpoint;
	}

	glm::vec3 Animation::SolveEularAngle(glm::qua<float> RotateQuaternion)
	{
		glm::vec3 Eular = glm::vec3(0.0f);
		glm::mat4 Matrix = glm::mat4_cast(RotateQuaternion);
		//  		float q0 = RotateQuaternion.w;
		//  		float q1 = RotateQuaternion.x;
		//  		float q2 = RotateQuaternion.y;
		//  		float q3 = RotateQuaternion.z;
		//   		float A13 = -2 * q0 * q2 + 2 * q1 * q3;
		//   		float A21 = -2 * q0 * q3 + 2 * q1 * q2;
		//   		float A22 = q0 * q0 - q1 * q1 + q2 * q2 - q3 * q3;
		//   		float A23 = 2 * q0 * q1 + 2 * q2 * q3;
		//   		float A33 = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;
		float A13 = Matrix[2][0];
		float A12 = Matrix[1][0];
		float A11 = Matrix[0][0];
		float A23 = Matrix[2][1];
		float A33 = Matrix[2][2];
		if (A11 >= 0 || A33 >= 0)
		{
			Eular.y = asin(A13);
		}
		else if (A11 < 0 && A33 < 0 && A13 >= 0)
		{
			Eular.y = PI - asin(A13);
		}
		else if (A11 < 0 && A33 < 0 && A13 <= 0)
		{
			Eular.y = -PI - asin(A13);
		}

		//if (cos(Eular.y) > -0.001 && cos(Eular.y) < 0.001)
		//{
			//Eular.x = lastEular.x;
			//Eular.z = lastEular.z;
		//}
		//else
		//{
			float temp = -A12 / A11;
			if (temp > 2 && A11 >= 0)
			{
				Eular.z = PI / 2 - atan(-A11 / A12);
			}
			if (temp > 2 && A11 <= 0)
			{
				Eular.z = -PI / 2 - atan(-A11 / A12);
			}
			if (temp >= 0 && temp <= 2 && A11 > 0)
			{
				Eular.z = atan(temp);
			}
			if (temp >= 0 && temp <= 2 && A11 < 0)
			{
				Eular.z = -PI + atan(temp);
			}
			if (temp > -2 && temp <= 0 && A11 > 0)
			{
				Eular.z = atan(temp);
			}
			if (temp > -2 && temp <= 0 && A11 < 0)
			{
				Eular.z = PI + atan(temp);
			}
			if (temp <= -2 && A11 >= 0)
			{
				Eular.z = -PI / 2 - atan(-A11 / A12);
			}
			if (temp <= -2 && A11 <= 0)
			{
				Eular.z = PI / 2 - atan(-A11 / A12);
			}

			temp = -A23 / A33;
			if (temp > 2 && A33 >= 0)
			{
				Eular.x = PI / 2 - atan(-A33 / A23);
			}
			if (temp > 2 && A33 <= 0)
			{
				Eular.x = -PI / 2 - atan(-A33 / A23);
			}
			if (temp >= 0 && temp <= 2 && A33 > 0)
			{
				Eular.x = atan(temp);
			}
			if (temp >= 0 && temp <= 2 && A33 < 0)
			{
				Eular.x = -PI + atan(temp);
			}
			if (temp > -2 && temp <= 0 && A33 > 0)
			{
				Eular.x = atan(temp);
			}
			if (temp > -2 && temp <= 0 && A33 < 0)
			{
				Eular.x = PI + atan(temp);
			}
			if (temp <= -2 && A33 >= 0)
			{
				Eular.x = -PI / 2 - atan(-A33 / A23);
			}
			if (temp <= -2 && A33 <= 0)
			{
				Eular.x = PI / 2 - atan(-A33 / A23);
			}
		//}
		// 		if (sin(Eular.x) > -0.001 && sin(Eular.x) < 0.001)
		// 		{
		// 			Eular.x = lastEular.x;
		// 		}
		// 		if (sin(Eular.z) > -0.001 && sin(Eular.z) < 0.001)
		// 		{
		// 			Eular.z = lastEular.z;
		// 		}
		return Eular;
	}

	glm::vec3 Animation::GetPathKeyPos(int index)
	{
		return m_Path_Pos[index];
	}

	glm::qua<float> Animation::GetPathKeyRotate(int index)
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

	float Animation::GetPathTotalTime(int index)
	{
		if (m_Path_Pos.size() > 1)
		{
			float totaltime = 0;
			for (int i = 0; i < index; i++)
			{
				totaltime += Path_Time[i];
			}
			return totaltime;
		}
		else
			return 0;
	}

	float Animation::GetPathKeyTime(int index)
	{
		return Path_Time[index];
	}

	void Animation::SetPathMode(glm::vec3 CircleCenter)
	{
		if (m_Path_Pos.size() > 1)
		{
			pathmodelist.push_back(pathmode);
			CircleCenterList.push_back(CircleCenter);
		}

	}

	void Animation::SetPathState2(std::string state2)
	{
		m_Path_State2.push_back(state2);
	}

	std::string Animation::GetPathKeyState2(int index)
	{
		return m_Path_State2[index];
	}

	void Animation::RemovePath()
	{
		if(m_Path_Pos.size()>0)
		{
			if (m_Path_Pos.size() > 1)
			{
				Path_Time.erase(Path_Time.end() - 1);
				pathmodelist.erase(pathmodelist.end() - 1);
				CircleCenterList.erase(CircleCenterList.end() - 1);
			}
			m_Path_Pos.erase(m_Path_Pos.end() - 1);
			m_Path_Rotate.erase(m_Path_Rotate.end() - 1);
			m_Path_HandPos.erase(m_Path_HandPos.end() - 1);
			m_Path_HandEular.erase(m_Path_HandEular.end() - 1);
			m_Path_State2.erase(m_Path_State2.end() - 1);
		}
		
	}

	int Animation::GetPathKeySize()
	{
		return m_Path_Pos.size();
	}

	int Animation::GetPathMode(int index)
	{
		return (int)pathmodelist[index];
	}

	glm::vec3 Animation::GetPathCircleCenter(int index)
	{
		return CircleCenterList[index];
	}

}
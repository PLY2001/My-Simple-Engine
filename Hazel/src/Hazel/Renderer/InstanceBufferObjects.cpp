#include "hzpch.h"
#include "InstanceBufferObjects.h"
namespace Hazel {
	void InstanceBufferObjects::AddObject(std::shared_ptr<Model>& model)
	{
		ObjectsAmount++;
		m_insbos.resize(ObjectsAmount);
		for (int i = 0; i < model->meshes.size(); i++)//��ģ���ж������ʱ��ÿ���������Լ��Ķ����������ID��Ҫ���ʵ�������黺��������ÿ��������������ϣ��ͱ������
		{
			m_insbos[ObjectsAmount - 1].push_back(NULL);
			m_insbos[ObjectsAmount - 1].back().reset(new InstanceBuffer(100 * sizeof(glm::mat4), NULL));
			m_insbos[ObjectsAmount - 1].back()->AddInstanceBuffermat4(model->meshes[i].vaID, 3);
		}
	}



	void InstanceBufferObjects::SetDatamat4(std::shared_ptr<Objects>& objects)
	{
		for (int j = 0; j < ObjectsAmount; j++)
		{
			for (int i = 0; i < m_insbos[j].size(); i++)
			{
				m_insbos[j][i]->SetDatamat4(sizeof(glm::mat4) * objects->GetAmount(j), objects->GetModelMatrix(j,i));
			}
		}
	}
}
#include "hzpch.h"
#include "InstanceBufferObjects.h"
namespace Hazel {
	void InstanceBufferObjects::AddObjects(std::shared_ptr<Objects>& objects)
	{
		ObjectsAmount = objects->GetObjectAmount();
		m_insbos.resize(ObjectsAmount);
		for(int j = 0; j < ObjectsAmount; j++)
		{
			for (int i = 0; i < objects->objects[j].m_Model->meshes.size(); i++)//该模型有多个网格时，每个网格都有自己的顶点数组对象ID，要想把实例化数组缓冲区绑定在每个顶点数组对象上，就必须遍历
			{
				m_insbos[j].push_back(NULL);
				m_insbos[j].back().reset(new InstanceBuffer(100 * sizeof(glm::mat4), NULL));
				m_insbos[j].back()->AddInstanceBuffermat4(objects->objects[j].m_Model->meshes[i].vaID, 3);
			}
		}
	}



	void InstanceBufferObjects::AddObject(std::shared_ptr<Objects>& objects)
	{
		ObjectsAmount = objects->GetObjectAmount();
		m_insbos.resize(ObjectsAmount);
		m_insbos[ObjectsAmount - 1].resize((objects->GetAmount(ObjectsAmount - 1)-1)*objects->objects.back().m_Model->meshes.size());
		for (int i = 0; i < objects->objects.back().m_Model->meshes.size(); i++)//该模型有多个网格时，每个网格都有自己的顶点数组对象ID，要想把实例化数组缓冲区绑定在每个顶点数组对象上，就必须遍历
		{
			m_insbos[ObjectsAmount - 1].push_back(NULL);
			m_insbos[ObjectsAmount - 1].back().reset(new InstanceBuffer(100 * sizeof(glm::mat4), NULL));
			m_insbos[ObjectsAmount - 1].back()->AddInstanceBuffermat4(objects->objects.back().m_Model->meshes[i].vaID, 3);
		}
	}

	void InstanceBufferObjects::SetDatamat4(std::shared_ptr<Objects>& objects)
	{
		ObjectsAmount = objects->GetObjectAmount();
		for (int j = 0; j < ObjectsAmount; j++)
		{
			for (int i = 0; i < m_insbos[j].size(); i++)
			{
				m_insbos[j][i]->SetDatamat4(sizeof(glm::mat4) * objects->GetAmount(j), objects->GetModelMatrix(j,i));
			}
		}
	}
}
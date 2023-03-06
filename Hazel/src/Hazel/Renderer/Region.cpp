#include "hzpch.h"
#include "Region.h"

namespace Hazel {
	Region::Region(glm::vec3 LastWorldPos,glm::vec3 WorldPos,std::shared_ptr<Shader>& shader) :m_shader(shader)
	{
		float Region[] = { LastWorldPos.x,0.1f,LastWorldPos.z,
						   LastWorldPos.x,0.1f,WorldPos.z,
						   WorldPos.x,0.1f,WorldPos.z,
		
						   LastWorldPos.x,0.1f,LastWorldPos.z,
						   WorldPos.x,0.1f,WorldPos.z,
						   WorldPos.x,0.1f,LastWorldPos.z
						 };

		m_va.reset(new VertexArray(vaID));
		m_vb.reset(new VertexBuffer(vbID, Region, 18 * sizeof(float)));

		VertexAttribLayout layout;//创建顶点属性布局实例
		layout.Push<GL_FLOAT>(3);//填入第一个属性布局，类型为float，每个点为3维向量

		m_va->AddBuffer(vbID, layout);//将所有属性布局应用于顶点缓冲区vb，并绑定在顶点数组对象va上

		m_va->Unbind();
		m_vb->Unbind();
	}

	void Region::Draw()
	{


		m_shader->Bind();
		m_va->Bind();

		glDrawArrays(GL_TRIANGLES, 0, 6);

		m_shader->Unbind();
		m_va->Unbind();
	}
}



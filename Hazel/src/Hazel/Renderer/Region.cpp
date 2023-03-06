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

		VertexAttribLayout layout;//�����������Բ���ʵ��
		layout.Push<GL_FLOAT>(3);//�����һ�����Բ��֣�����Ϊfloat��ÿ����Ϊ3ά����

		m_va->AddBuffer(vbID, layout);//���������Բ���Ӧ���ڶ��㻺����vb�������ڶ����������va��

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



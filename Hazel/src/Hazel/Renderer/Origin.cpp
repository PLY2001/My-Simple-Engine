#include "hzpch.h"
#include "Origin.h"

namespace Hazel {
	Origin::Origin(std::shared_ptr<Shader>& shader) :m_shader(shader)
	{
		float Origin[] = { 1.0f,0.0f,0.0f,
						   0.0f,1.0f,0.0f,
						   0.0f,0.0f,1.0f };
		

		m_va.reset(new VertexArray(vaID));
		m_vb.reset(new VertexBuffer(vbID, Origin, 9 * sizeof(float)));

		VertexAttribLayout layout;//�����������Բ���ʵ��
		layout.Push<GL_FLOAT>(3);//�����һ�����Բ��֣�����Ϊfloat��ÿ����Ϊ3ά����

		m_va->AddBuffer(vbID, layout);//���������Բ���Ӧ���ڶ��㻺����vb�������ڶ����������va��

		m_va->Unbind();
		m_vb->Unbind();
	}

	void Origin::Draw()
	{
		

		m_shader->Bind();
		m_va->Bind();
		
		glDrawArrays(GL_POINTS, 0, 3);
		
		m_shader->Unbind();
		m_va->Unbind();
	}
}



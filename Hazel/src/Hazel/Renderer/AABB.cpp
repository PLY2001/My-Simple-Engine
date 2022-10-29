#include "hzpch.h"
#include "AABB.h"

namespace Hazel {
	AABB::AABB(std::shared_ptr<Shader>& shader):m_shader(shader)
	{

	}

	void AABB::Draw(glm::vec3 MinPos, glm::vec3 MaxPos)
	{
		float XMin = MinPos.x;
		float XMax = MaxPos.x;
		float YMin = MinPos.y;
		float YMax = MaxPos.y;
		float ZMin = MinPos.z;
		float ZMax = MaxPos.z;

		float AABBVertices[] = {
			// positions          
			XMin, YMax, ZMin,
			XMin, YMin, ZMin,
			XMax, YMin, ZMin,
			XMax, YMin, ZMin,
			XMax, YMax, ZMin,
			XMin, YMax, ZMin,

			XMin, YMin, ZMax,
			XMin, YMin, ZMin,
			XMin, YMax, ZMin,
			XMin, YMax, ZMin,
			XMin, YMax, ZMax,
			XMin, YMin, ZMax,

			XMax, YMin, ZMin,
			XMax, YMin, ZMax,
			XMax, YMax, ZMax,
			XMax, YMax, ZMax,
			XMax, YMax, ZMin,
			XMax, YMin, ZMin,

			XMin, YMin, ZMax,
			XMin, YMax, ZMax,
			XMax, YMax, ZMax,
			XMax, YMax, ZMax,
			XMax, YMin, ZMax,
			XMin, YMin, ZMax,

			XMin, YMax, ZMin,
			XMax, YMax, ZMin,
			XMax, YMax, ZMax,
			XMax, YMax, ZMax,
			XMin, YMax, ZMax,
			XMin, YMax, ZMin,

			XMin, YMin, ZMin,
			XMin, YMin, ZMax,
			XMax, YMin, ZMin,
			XMax, YMin, ZMin,
			XMin, YMin, ZMax,
			XMax, YMin, ZMax
		};
		unsigned int vaID;//VertexArray
		unsigned int vbID;

		VertexArray va(vaID);
		VertexBuffer vb(vbID, AABBVertices, 108 * sizeof(float));

		VertexAttribLayout layout;//�����������Բ���ʵ��
		layout.Push<GL_FLOAT>(3);//�����һ�����Բ��֣�����Ϊfloat��ÿ����Ϊ3ά����

		va.AddBuffer(vbID, layout);//���������Բ���Ӧ���ڶ��㻺����vb�������ڶ����������va��

		va.Unbind();
		vb.Unbind();

		m_shader->Bind();
		va.Bind();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		m_shader->Unbind();
		va.Unbind();
	}
}



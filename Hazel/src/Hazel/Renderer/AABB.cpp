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

		VertexAttribLayout layout;//创建顶点属性布局实例
		layout.Push<GL_FLOAT>(3);//填入第一个属性布局，类型为float，每个点为3维向量

		va.AddBuffer(vbID, layout);//将所有属性布局应用于顶点缓冲区vb，并绑定在顶点数组对象va上

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



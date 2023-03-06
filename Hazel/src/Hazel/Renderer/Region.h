#pragma once
#include "Buffer.h"

namespace Hazel {

	class Region
	{
	public:
		Region(glm::vec3 LastWorldPos, glm::vec3 WorldPos, std::shared_ptr<Shader>& shader);
		~Region() = default;
		void Draw();
	private:
		std::shared_ptr<Shader> m_shader;
		std::unique_ptr<VertexArray> m_va;
		std::unique_ptr<VertexBuffer> m_vb;
		unsigned int vaID;//VertexArray
		unsigned int vbID;

	};
}

